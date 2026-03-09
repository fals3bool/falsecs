#include <ecs/component.h>
#include <ecs/system.h>
#include <math.h>

// https://dyn4j.org/2010/04/gjk-gilbert-johnson-keerthi/
// https://dyn4j.org/2010/05/epa-expanding-polytope-algorithm/

// Apply impulses to rigidbodies
static void ResolveCollision(Collision *input, Transform2 *ta, RigidBody *ra,
                             Transform2 *tb, RigidBody *rb) {
  float invmassA = (ra && ra->type == BodyDynamic) ? ra->invmass : 0;
  float invmassB = (rb && rb->type == BodyDynamic) ? rb->invmass : 0;
  if (invmassA + invmassB == 0)
    return;

  float deltaMagnitude = input->distance / (invmassA + invmassB);
  Vector2 delta = Vector2Scale(input->normal, deltaMagnitude);
  ta->position = Vector2Subtract(ta->position, Vector2Scale(delta, invmassA));
  tb->position = Vector2Add(tb->position, Vector2Scale(delta, invmassB));

  Vector2 deltaSpeed = Vector2Subtract(rb ? rb->speed : (Vector2){0, 0},
                                       ra ? ra->speed : (Vector2){0, 0});
  float speedAlongNormal = Vector2DotProduct(deltaSpeed, input->normal);
  if (speedAlongNormal > 0) // rigidbodies separated
    return;

  float e = 0;
  float impulseMagnitute = -(1 + e) * speedAlongNormal / (invmassA + invmassB);
  Vector2 impulse = Vector2Scale(input->normal, impulseMagnitute);
  if (ra && ra->type == BodyDynamic)
    ra->speed = Vector2Subtract(ra->speed, Vector2Scale(impulse, invmassA));
  if (rb && rb->type == BodyDynamic)
    rb->speed = Vector2Add(rb->speed, Vector2Scale(impulse, invmassB));
}

// OnCollision events
static void HandleEvents(ECS *ecs, Entity self, Entity other, Collision *col) {
  CollisionListener *listener_a = GetComponent(ecs, self, CollisionListener);
  if (listener_a) {
    CollisionEvent event = {self, other, *col};
    listener_a->OnCollision(ecs, &event);
  }

  CollisionListener *listener_b = GetComponent(ecs, other, CollisionListener);
  if (listener_b) {
    CollisionEvent event = {
        other, self, {Vector2Negate(col->normal), col->distance}};
    listener_b->OnCollision(ecs, &event);
  }
}

// Math
static Vector2 Vector2TripleProduct(Vector2 a, Vector2 b, Vector2 c) {
  Vector2 res = {0};
  float ac = a.x * c.x + a.y * c.y; // dot(a, c)
  float bc = b.x * c.x + b.y * c.y; // dot(b, c)
  res.x = b.x * ac - a.x * bc;
  res.y = b.y * ac - a.y * bc;
  return res;
}

static Vector2 Vector2Perpendicular(Vector2 v) {
  Vector2 res = {v.y, -v.x};
  return res;
}

// Furthest point index of a shape based on a direction 'd'
static uint8_t ColliderFurthestIndex(Collider *c, Vector2 d) {
  float max = Vector2DotProduct(d, c->vx[0]);
  uint8_t index = 0;
  for (uint8_t i = 1; i < c->vertices; i++) {
    float prod = Vector2DotProduct(d, c->vx[i]);
    if (prod > max) {
      max = prod;
      index = i;
    }
  }
  return index;
}

// Minkowski Difference Support Point by a given direction 'd'
static Vector2 Support(Collider *A, Collider *B, Vector2 d) {
  uint8_t a = ColliderFurthestIndex(A, d);
  uint8_t b = ColliderFurthestIndex(B, Vector2Negate(d));
  return Vector2Subtract(A->vx[a], B->vx[b]);
}

typedef struct {
  Vector2 vx[3];
  bool overlap;
} Simplex;

typedef struct {
  int index;
  Vector2 normal;
  float distance;
} Edge;

static Edge SimplexClosestEdge(Vector2 *poly, int count) {
  Edge closest;
  closest.distance = INFINITY;

  for (int i = 0; i < count; i++) {
    int j = (i + 1) % count;
    Vector2 a = poly[i];
    Vector2 b = poly[j];

    Vector2 edge = Vector2Subtract(b, a);
    Vector2 normal = Vector2Normalize(Vector2Perpendicular(edge));
    if (Vector2DotProduct(normal, a) < 0)
      normal = Vector2Negate(normal);

    float dist = Vector2DotProduct(normal, a);
    if (dist < closest.distance) {
      closest.distance = dist;
      closest.normal = normal;
      closest.index = j;
    }
  }

  return closest;
}

static void InsertVertex(Vector2 *poly, int *count, int index, Vector2 p) {
  for (int i = *count; i > index; i--)
    poly[i] = poly[i - 1];
  poly[index] = p;
  (*count)++;
}

static Collision EPA(Collider *ca, Collider *cb, Simplex *simplex) {
  int max_verts = 64;
  float epsilon = 0.001f;

  Vector2 poly[max_verts];
  int count = 3;

  poly[0] = simplex->vx[0];
  poly[1] = simplex->vx[1];
  poly[2] = simplex->vx[2];

  while (true) {
    Edge e = SimplexClosestEdge(poly, count);
    Vector2 p = Support(ca, cb, e.normal);
    float d = Vector2DotProduct(p, e.normal);

    if (d - e.distance < epsilon) {
      Collision c;
      c.normal = e.normal;
      c.distance = d;
      return c;
    }

    if (count >= max_verts)
      break;
    InsertVertex(poly, &count, e.index, p);
  }

  Collision c = {0};
  return c;
}

static Simplex GJK(Collider *ca, Transform2 *ta, Collider *cb, Transform2 *tb) {
  Vector2 a, b, c;        // simplex points
  Vector2 d;              // direction
  Vector2 ao, ab, ac;     // aristas
  Vector2 abperp, acperp; // perpendicular vectors
  Simplex simplex = {0};  // simplex
  uint8_t cur = 0;        // simplex current vertex

  d = Vector2Subtract(ta->position, tb->position);
  if ((d.x == 0) && (d.y == 0)) // arbitrary if (0,0)
    d.x = 1.f;

  a = simplex.vx[0] = Support(ca, cb, d);
  if (Vector2DotProduct(a, d) <= 0)
    return simplex; // no collision

  d = Vector2Negate(a);

  while (true) {
    a = simplex.vx[++cur] = Support(ca, cb, d);
    if (Vector2DotProduct(a, d) <= 0)
      return simplex; // no collision

    ao = Vector2Negate(a); // A - (0,0)

    // Line Case
    if (cur < 2) {
      b = simplex.vx[0];
      ab = Vector2Subtract(b, a);
      d = Vector2TripleProduct(ab, ao, ab);
      if (Vector2LengthSqr(d) == 0)
        d = Vector2Perpendicular(ab);
      continue;
    }

    // Triangle Case
    b = simplex.vx[1];
    c = simplex.vx[0];
    ab = Vector2Subtract(b, a);
    ac = Vector2Subtract(c, a);
    acperp = Vector2TripleProduct(ab, ac, ac);

    if (Vector2DotProduct(acperp, ao) >= 0) {
      d = acperp;
    } else {
      abperp = Vector2TripleProduct(ac, ab, ab);
      if (Vector2DotProduct(abperp, ao) < 0) { // collision
        simplex.overlap = true;
        return simplex;
      }
      // if no collision:
      simplex.vx[0] = simplex.vx[1]; // swap
      d = abperp;
    }

    simplex.vx[1] = simplex.vx[2]; // swap
    --cur;
  }

  return simplex;
}

void CollisionSystem(ECS *ecs, Entity self) {
  Transform2 *ta = GetComponent(ecs, self, Transform2);
  Collider *ca = GetComponent(ecs, self, Collider);
  RigidBody *ra = GetComponent(ecs, self, RigidBody);
  uint8_t la = EcsEntityData(ecs, self)->layer;

  Signature mask = EcsSignature(ecs, Transform2, Collider);
  for (Entity other = self + 1; other < EcsEntityCount(ecs); ++other) {
    if (!EcsHasComponents(ecs, other, mask))
      continue;
    if (!EntityIsActive(ecs, other))
      continue;

    uint8_t lb = EcsEntityData(ecs, other)->layer;
    if (!LayerIncludes(ecs, la, lb))
      continue;

    Transform2 *tb = GetComponent(ecs, other, Transform2);
    Collider *cb = GetComponent(ecs, other, Collider);
    RigidBody *rb = GetComponent(ecs, other, RigidBody);

    Simplex simplex = GJK(ca, ta, cb, tb);
    ca->overlap |= simplex.overlap;
    cb->overlap |= simplex.overlap;

    if (simplex.overlap) {
      Collision collision = EPA(ca, cb, &simplex);
      HandleEvents(ecs, self, other, &collision);
      ResolveCollision(&collision, ta, ra, tb, rb);
    }
  }
}
