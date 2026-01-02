#include <ecs/component.h>
#include <ecs/system.h>

#include <raymath.h>

void TransformColliderSystem(ECS *ecs, Entity e) {
  Transform2 *t = EcsGet(ecs, e, Transform2);
  Collider *c = EcsGet(ecs, e, Collider);

  float angle = t->rotation;
  for (uint8_t i = 0; i < c->vertices; i++) {
    c->vx[i].x =
        c->md[i].x * cosf(angle) - c->md[i].y * sinf(angle) + t->position.x;
    c->vx[i].y =
        c->md[i].x * sinf(angle) + c->md[i].y * cosf(angle) + t->position.y;
  }
  c->overlap = false;
}

void DebugColliderSystem(ECS *ecs, Entity e) {
  Collider *col = EcsGet(ecs, e, Collider);
  for (uint8_t i = 0; i < col->vertices; i++) {
    Vector2 p = col->vx[i];
    Vector2 q = col->vx[(i + 1) % col->vertices];
    DrawLineEx(p, q, 2, col->overlap ? RED : SKYBLUE);
  }
}

// COLLISIONS

static uint8_t SatProj(Transform2 *ta, Collider *ca, Transform2 *tb,
                       Collider *cb, float *min_distance, Vector2 *axis) {
  for (uint8_t i = 0; i < ca->vertices; i++) {
    uint8_t j = (i + 1) % ca->vertices;

    Vector2 proj = {-(ca->vx[j].y - ca->vx[i].y), ca->vx[j].x - ca->vx[i].x};
    proj = Vector2Normalize(proj);

    float min_r1 = INFINITY, max_r1 = -INFINITY;
    for (uint8_t p = 0; p < ca->vertices; p++) {
      float q = (ca->vx[p].x * proj.x + ca->vx[p].y * proj.y);
      min_r1 = fminf(min_r1, q);
      max_r1 = fmaxf(max_r1, q);
    }

    float min_r2 = INFINITY, max_r2 = -INFINITY;
    for (uint8_t p = 0; p < cb->vertices; p++) {
      float q = (cb->vx[p].x * proj.x + cb->vx[p].y * proj.y);
      min_r2 = fminf(min_r2, q);
      max_r2 = fmaxf(max_r2, q);
    }

    float over = fminf(max_r1, max_r2) - fmaxf(min_r1, min_r2);
    if (over <= 0)
      return false;

    if (over < *min_distance) {
      *min_distance = over;
      *axis = proj;
    }
  }
  return true;
}

uint8_t CollisionSat(Transform2 *ta, Collider *ca, Transform2 *tb, Collider *cb,
                     Collision *output) {
  float distance = INFINITY;
  Vector2 proj = {0, 0};

  if (!SatProj(ta, ca, tb, cb, &distance, &proj))
    return false;

  if (!SatProj(tb, cb, ta, ca, &distance, &proj))
    return false;

  Vector2 delta = {tb->position.x - ta->position.x,
                   tb->position.y - ta->position.y};

  if (delta.x * proj.x + delta.y * proj.y < 0.0f) {
    proj.x = -proj.x;
    proj.y = -proj.y;
  }

  output->normal = Vector2Normalize(proj);
  output->distance = distance;

  return true;
}

void ResolveCollision(Collision *input, Transform2 *ta, RigidBody *ra,
                      Transform2 *tb, RigidBody *rb) {
  float invmassA = (ra && ra->type == RIGIDBODY_DYNAMIC) ? ra->invmass : 0;
  float invmassB = (rb && rb->type == RIGIDBODY_DYNAMIC) ? rb->invmass : 0;
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
  if (ra && ra->type == RIGIDBODY_DYNAMIC)
    ra->speed = Vector2Subtract(ra->speed, Vector2Scale(impulse, invmassA));
  if (rb && rb->type == RIGIDBODY_DYNAMIC)
    rb->speed = Vector2Add(rb->speed, Vector2Scale(impulse, invmassB));
}

void CollisionSystem(ECS *ecs, Entity self) {
  Transform2 *ta = EcsGet(ecs, self, Transform2);
  Collider *ca = EcsGet(ecs, self, Collider);
  RigidBody *ra = EcsGet(ecs, self, RigidBody);

  Signature mask = EcsSignature(ecs, Transform2) & EcsSignature(ecs, Collider);
  for (Entity other = self + 1; other < EcsEntityCount(ecs); ++other) {
    if (!EcsHasComponent(ecs, other, mask))
      continue;
    if (!EcsEntityIsActive(ecs, other))
      continue;
    Transform2 *tb = EcsGet(ecs, other, Transform2);
    Collider *cb = EcsGet(ecs, other, Collider);
    RigidBody *rb = EcsGet(ecs, other, RigidBody);

    Collision collision;
    uint8_t overlap = CollisionSat(ta, ca, tb, cb, &collision);

    if (overlap) {
      if (ca->OnCollision) {
        CollisionEvent event = {self, other, collision};
        ca->OnCollision(&event);
      }
      if (cb->OnCollision) {
        CollisionEvent event = {
            other, self, {Vector2Negate(collision.normal), collision.distance}};
        cb->OnCollision(&event);
      }
    }

    ca->overlap |= overlap;
    cb->overlap |= overlap;
    overlap &= ca->solid && cb->solid;

    if (overlap)
      ResolveCollision(&collision, ta, ra, tb, rb);
  }
}
