#include <ecs/component.h>
#include <ecs/system.h>

#include <assert.h>

// ######### //
//  SIMPLEX  //
// ######### //

typedef struct {
  Vector3 v[4];
  uint8_t size;
} Simplex;
// Simplex = {D, C, B, A}

static void SimplexPush(Simplex *s, Vector3 v) {
  assert(s->size < 5 && "Overflow Simplex size");
  s->v[s->size++] = v;
}

static void SimplexPop(Simplex *s, uint8_t at) {
  assert(s->size > 0 && "Empty Simplex");
  for (uint8_t i = at; i < s->size - 1; i++)
    s->v[i] = s->v[i + 1];
  s->size--;
}

static void SimplexSwap(Simplex *s, uint8_t a, uint8_t b) {
  assert(a < s->size && b < s->size && "Index out of Simplex Bounds");
  Vector3 tmp = s->v[a];
  s->v[a] = s->v[b];
  s->v[b] = tmp;
}

// ##### //
//  GJK  //
// ##### //

// A x B x C = B(A.C) - C(A.B)
static Vector3 Vector3TripleProduct(Vector3 a, Vector3 b, Vector3 c) {
  return Vector3Subtract(Vector3Scale(b, Vector3DotProduct(a, c)),
                         Vector3Scale(c, Vector3DotProduct(a, b)));
}

static uint8_t FurthestPoint(Collider *a, Vector3 d) {
  float fur = Vector3DotProduct(a->vx[0], d);
  uint8_t furi = 0;
  for (uint8_t i = 1; i < a->vertices; i++) {
    float dot = Vector3DotProduct(a->vx[i], d);
    if (dot > fur) {
      fur = dot;
      furi = i;
    }
  }
  return furi;
}

static Vector3 Support(Collider *a, Collider *b, Vector3 d) {
  uint8_t i = FurthestPoint(a, d);
  uint8_t j = FurthestPoint(b, Vector3Negate(d));
  return Vector3Subtract(a->vx[i], b->vx[j]);
}

static bool DoSimplex(Simplex *s, Vector3 *d) {
  switch (s->size) {
  // line case
  case 2: {
    Vector3 AO = Vector3Negate(s->v[1]); // (0,0) - A
    Vector3 AB = Vector3Subtract(s->v[0], s->v[1]);

    if (Vector3DotProduct(AB, AO) > 0) {
      *d = Vector3TripleProduct(AB, AO, AB);
      if (Vector3LengthSqr(*d) == 0)
        *d = Vector3Perpendicular(AB);
    } else {
      SimplexPop(s, 0);
      *d = AO;
    }
  } break;
    // triangle case
  case 3: {
    Vector3 AB = Vector3Subtract(s->v[1], s->v[2]);
    Vector3 AC = Vector3Subtract(s->v[0], s->v[2]);
    Vector3 AO = Vector3Negate(s->v[2]);
    Vector3 ABC = Vector3CrossProduct(AB, AC);

    // CASO 1: Lado AC
    if (Vector3DotProduct(Vector3CrossProduct(ABC, AC), AO) > 0) {
      if (Vector3DotProduct(AC, AO) > 0) {
        SimplexPop(s, 1);
        *d = Vector3TripleProduct(AC, AO, AC);
      } else if (Vector3DotProduct(AB, AO) > 0) {
        SimplexPop(s, 0);
        *d = Vector3TripleProduct(AB, AO, AB);
      } else {
        SimplexPop(s, 0);
        SimplexPop(s, 0);
        *d = AO;
      }
    }

    // CASO 2: Lado AB
    else if (Vector3DotProduct(Vector3CrossProduct(AB, ABC), AO) > 0) {
      if (Vector3DotProduct(AB, AO) > 0) {
        SimplexPop(s, 0);
        *d = Vector3TripleProduct(AB, AO, AB);
      } else {
        SimplexPop(s, 0);
        SimplexPop(s, 0);
        *d = AO;
      }
    }

    // CASO 3: Arriba o Abajo // 2D -> 
    else {
      if (Vector3DotProduct(ABC, AO) > 0) {
        *d = ABC;
      } else {
        SimplexSwap(s, 0, 1);
        *d = Vector3Negate(ABC);
      }
    }
  } break;
    // tetraedro case
  case 4: {
    Vector3 AB = Vector3Subtract(s->v[2], s->v[3]);
    Vector3 AC = Vector3Subtract(s->v[1], s->v[3]);
    Vector3 AD = Vector3Subtract(s->v[0], s->v[3]);
    Vector3 AO = Vector3Negate(s->v[3]);
    Vector3 ABC = Vector3CrossProduct(AB, AC);
    Vector3 ACD = Vector3CrossProduct(AC, AD);
    Vector3 ADB = Vector3CrossProduct(AD, AB);

    // Cara ABC
    if (Vector3DotProduct(ABC, AO) > 0) {
      SimplexPop(s, 0);
      return DoSimplex(s, d);
    }
    // Cara ACD
    if (Vector3DotProduct(ACD, AO) > 0) {
      SimplexPop(s, 2);
      return DoSimplex(s, d);
    }
    // Cara ADB
    if (Vector3DotProduct(ADB, AO) > 0) {
      SimplexPop(s, 1);
      return DoSimplex(s, d);
    }
    return true; // 
  } break;
  default:
    // do nothing
    break;
  }
  return false;
}

static bool GJK(Transform *ta, Collider *ca, Transform *tb, Collider *cb,
                Simplex *s) {

  // P1
  Vector3 D = Vector3Subtract(ta->translation, tb->translation);
  Vector3 A = Support(ca, cb, D);
  if (Vector3DotProduct(A, D) <= 0)
    return false;

  SimplexPush(s, A);
  D = Vector3Negate(A);

  while (true) {
    Vector3 P = Support(ca, cb, D);
    if (Vector3DotProduct(P, D) <= 0)
      return false;

    SimplexPush(s, P);
    if (DoSimplex(s, &D))
      break;
  }

  return true;
}

// ########## //
//  POLYTOPE  //
// ########## //

typedef struct {
  uint8_t a, b;
} Edge;

static int EdgeFind(Edge *edges, uint8_t len, uint8_t a, uint8_t b) {
  for (uint8_t i = 0; i < len; ++i) {
    if (edges[i].a == a && edges[i].b == b)
      return i;
  }
  return -1;
}

typedef struct {
  uint8_t v[3];
  Vector3 normal;
  float distance;
} Face;

typedef struct {
  Face f[64];
  Vector3 v[32];
  uint8_t faces, vertices;
} Polytope;

static void PolytopeAddFace(Polytope *poly, uint8_t a, uint8_t b, uint8_t c) {
  Vector3 v1 = Vector3Subtract(poly->v[b], poly->v[a]);
  Vector3 v2 = Vector3Subtract(poly->v[c], poly->v[a]);
  Vector3 n = Vector3Normalize(Vector3CrossProduct(v1, v2));
  float d = Vector3DotProduct(n, poly->v[a]);
  if (d < 0) {
    d = -d;
    n = Vector3Negate(n);
    poly->f[poly->faces++] = (Face){{a, c, b}, n, d};
  } else {
    poly->f[poly->faces++] = (Face){{a, b, c}, n, d};
  }
}

static void PolytopeRemoveFace(Polytope *poly, uint8_t f) {
  assert(poly->faces > f);
  for (int i = f; i < poly->faces - 1; i++)
    poly->f[i] = poly->f[i + 1];
  poly->faces--;
}

static Polytope PolytopeCreate(const Simplex *s) {
  assert(s->size == 4 && "Incomplete simplex");
  Polytope poly = {{0}, {s->v[0], s->v[1], s->v[2], s->v[3]}, 0, 4};
  PolytopeAddFace(&poly, 0, 1, 2);
  PolytopeAddFace(&poly, 0, 1, 3);
  PolytopeAddFace(&poly, 0, 2, 3);
  PolytopeAddFace(&poly, 1, 2, 3);
  return poly;
}

// ##### //
//  EPA  //
// ##### //

static void EPAExpand(Polytope *poly, Vector3 p) {
  Edge ed[64];
  uint8_t edges = 0;
  // for face in polytope:
  for (uint8_t i = 0; i < poly->faces;) {
    assert(edges < 64);
    uint8_t t = poly->f[i].v[0];
    Vector3 w = Vector3Subtract(p, poly->v[t]);
    // f sees p: if (p-t).n > 0
    if (Vector3DotProduct(poly->f[i].normal, w) > 0) {
      // for edge in face: where edge = (v, (v+1)%3)
      for (uint8_t vi = 0; vi < 3; ++vi) {
        uint8_t ui = (vi + 1) % 3;
        uint8_t v = poly->f[i].v[vi];
        uint8_t u = poly->f[i].v[ui];
        // if edge in edges:
        int e;
        // find inverted edge (v,u) -> (u,v)
        if ((e = EdgeFind(ed, edges, u, v)) == -1) { // push
          ed[edges++] = (Edge){v, u};
        } else { // remove
          for (int ei = e; ei < edges - 1; ++ei)
            ed[ei] = ed[ei + 1];
          edges--;
        }
      }
      PolytopeRemoveFace(poly, i);

    } else {
      ++i;
    }
  }

  uint8_t i = poly->vertices++;
  poly->v[i] = p;
  // for edge in edges:
  for (int e = 0; e < edges; ++e)
    PolytopeAddFace(poly, ed[e].a, ed[e].b, i);
}

static uint8_t EPAClosestFace(const Polytope *poly) {
  uint8_t min = 0;
  float d = poly->f[0].distance;
  for (int i = 1; i < poly->faces; ++i) {
    if (poly->f[i].distance < d) {
      d = poly->f[i].distance;
      min = i;
    }
  }
  return min;
}

static float epsilon = 0.001f;

static Collision EPA(Collider *ca, Collider *cb, Simplex *s) {
  uint8_t iter = 0;
  Polytope poly = PolytopeCreate(s);

  while (poly.vertices < 32 && iter < 64) {
    uint8_t i = EPAClosestFace(&poly);
    Vector3 n = poly.f[i].normal;
    float d = poly.f[i].distance;

    Vector3 p = Support(ca, cb, n);
    float di = Vector3DotProduct(p, n);
    if (di - d <= epsilon) {
      Collision c = {n, d};
      return c;
    } else {
      EPAExpand(&poly, p);
    }
    iter++;
  }

  Collision c = {0};
  return c;
}

void ResolveCollision(Collision *input, Transform *ta, RigidBody *ra,
                      Transform *tb, RigidBody *rb) {
  float invmassA = (ra && ra->type == BodyDynamic) ? ra->invmass : 0;
  float invmassB = (rb && rb->type == BodyDynamic) ? rb->invmass : 0;
  if (invmassA + invmassB == 0)
    return;

  float deltaMagnitude = input->distance / (invmassA + invmassB);
  Vector3 delta = Vector3Scale(input->normal, deltaMagnitude);
  ta->translation =
      Vector3Subtract(ta->translation, Vector3Scale(delta, invmassA));
  tb->translation = Vector3Add(tb->translation, Vector3Scale(delta, invmassB));

  Vector3 deltaSpeed = Vector3Subtract(rb ? rb->speed : (Vector3){0},
                                       ra ? ra->speed : (Vector3){0});
  float speedAlongNormal = Vector3DotProduct(deltaSpeed, input->normal);
  if (speedAlongNormal > 0) // rigidbodies separated
    return;

  float e = 0;
  float impulseMagnitute = -(1 + e) * speedAlongNormal / (invmassA + invmassB);
  Vector3 impulse = Vector3Scale(input->normal, impulseMagnitute);
  if (ra && ra->type == BodyDynamic)
    ra->speed = Vector3Subtract(ra->speed, Vector3Scale(impulse, invmassA));
  if (rb && rb->type == BodyDynamic)
    rb->speed = Vector3Add(rb->speed, Vector3Scale(impulse, invmassB));
}

void HandleCollisionEvents(ECS *ecs, Entity self, Entity other,
                           Collision *collision) {
  CollisionListener *listener_a = GetComponent(ecs, self, CollisionListener);
  if (listener_a) {
    CollisionEvent event = {self, other, *collision};
    listener_a->OnCollision(ecs, &event);
  }

  CollisionListener *listener_b = GetComponent(ecs, other, CollisionListener);
  if (listener_b) {
    CollisionEvent event = {
        other, self, {Vector3Negate(collision->normal), collision->distance}};
    listener_b->OnCollision(ecs, &event);
  }
}

void CollisionSystem(ECS *ecs, Entity self) {
  Transform *ta = GetComponent(ecs, self, Transform);
  Collider *ca = GetComponent(ecs, self, Collider);
  RigidBody *ra = GetComponent(ecs, self, RigidBody);
  uint8_t la = EcsEntityData(ecs, self)->layer;

  Signature mask = EcsSignature(ecs, Transform, Collider);
  for (Entity other = self + 1; other < EcsEntityCount(ecs); ++other) {
    if (!EcsHasComponents(ecs, other, mask))
      continue;
    if (!EntityIsActive(ecs, other))
      continue;

    Transform *tb = GetComponent(ecs, other, Transform);
    Collider *cb = GetComponent(ecs, other, Collider);
    RigidBody *rb = GetComponent(ecs, other, RigidBody);
    uint8_t lb = EcsEntityData(ecs, other)->layer;

    Collision collision;
    Simplex s = {0};
    uint8_t overlap = LayerIncludes(ecs, la, lb) && GJK(ta, ca, tb, cb, &s);

    ca->overlap |= overlap;
    cb->overlap |= overlap;
    overlap &= ca->solid && cb->solid;

    if (overlap) {
      collision = EPA(ca, cb, &s);
      HandleCollisionEvents(ecs, self, other, &collision);
      ResolveCollision(&collision, ta, ra, tb, rb);
    }
  }
}
