#include <ecs/component.h>
#include <ecs/system.h>

#include <math.h>
#include <raymath.h>
#include <stdint.h>

void ecs_transform_collider_system(Registry *r, Entity e) {
  Transform2 *t = ecs_get(r, e, Transform2);
  Collider *c = ecs_get(r, e, Collider);

  float angle = t->rotation + c->rot;
  for (uint8_t i = 0; i < c->vertices; i++) {
    c->vx[i].x = c->md[i].x * cosf(angle) - c->md[i].y * sinf(angle) +
                 t->position.x + c->origin.x;
    c->vx[i].y = c->md[i].x * sinf(angle) + c->md[i].y * cosf(angle) +
                 t->position.y + c->origin.y;
    c->overlap = false;
  }
}

void ecs_debug_collider_system(Registry *r, Entity e) {
  Collider *col = ecs_get(r, e, Collider);
  for (uint8_t i = 0; i < col->vertices; i++) {
    Vector2 p = col->vx[i];
    Vector2 q = col->vx[(i + 1) % col->vertices];
    DrawLineEx(p, q, 2, col->overlap ? RED : SKYBLUE);
  }
}

// COLLISIONS

typedef struct {
  Transform2 *ta, *tb;
  Vector2 normal;
  Vector2 contact;
  float distance;
} Collision;

static uint8_t sat_proj(Transform2 *ta, Collider *ca, Transform2 *tb,
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

uint8_t collision_sat(Transform2 *ta, Collider *ca, Transform2 *tb,
                      Collider *cb, Collision *output) {
  float distance = INFINITY;
  Vector2 proj = {0, 0};

  if (!sat_proj(ta, ca, tb, cb, &distance, &proj))
    return false;

  if (!sat_proj(tb, cb, ta, ca, &distance, &proj))
    return false;

  Vector2 delta = {tb->position.x - ta->position.x,
                   tb->position.y - ta->position.y};

  if (delta.x * proj.x + delta.y * proj.y < 0.0f) {
    proj.x = -proj.x;
    proj.y = -proj.y;
  }

  output->normal = Vector2Normalize(proj);
  output->distance = distance;
  output->ta = ta;
  output->tb = tb;

  return true;
}

uint8_t collision_overlap(Vector2 p, Collider *a, Collider *b) {
  // check diagonals of polygon collider A:
  for (uint8_t i = 0; i < a->vertices; i++) {
    Vector2 line_r1s = p;
    Vector2 line_r1e = a->vx[i];

    // against edges of polygon collider B:
    for (uint8_t j = 0; j < b->vertices; j++) {
      Vector2 line_r2s = b->vx[j];
      Vector2 line_r2e = b->vx[(j + 1) % b->vertices];

      float h = (line_r2e.x - line_r2s.x) * (line_r1s.y - line_r1e.y) -
                (line_r1s.x - line_r1e.x) * (line_r2e.y - line_r2s.y);
      float t1 = ((line_r2s.y - line_r2e.y) * (line_r1s.x - line_r2s.x) +
                  (line_r2e.x - line_r2s.x) * (line_r1s.y - line_r2s.y)) /
                 h;
      float t2 = ((line_r1s.y - line_r1e.y) * (line_r1s.x - line_r2s.x) +
                  (line_r1e.x - line_r1s.x) * (line_r1s.y - line_r2s.y)) /
                 h;

      if (t1 >= 0.0f && t1 < 1.0f && t2 >= 0.0f && t2 < 1.0f)
        return true;
    }
  }
  return false;
}

uint8_t collision_rb(Vector2 pA, Vector2 pB, Collider *cA, Collider *cB,
                     RigidBody *rbA, RigidBody *rbB) {
  // A:
  for (uint8_t i = 0; i < cA->vertices; i++) {
    Vector2 line_r1s = pA;
    Vector2 line_r1e = cA->vx[i];

    // B:
    for (uint8_t j = 0; j < cB->vertices; j++) {
      Vector2 line_r2s = cB->vx[j];
      Vector2 line_r2e = cB->vx[(j + 1) % cB->vertices];

      float h = (line_r2e.x - line_r2s.x) * (line_r1s.y - line_r1e.y) -
                (line_r1s.x - line_r1e.x) * (line_r2e.y - line_r2s.y);
      float t1 = ((line_r2s.y - line_r2e.y) * (line_r1s.x - line_r2s.x) +
                  (line_r2e.x - line_r2s.x) * (line_r1s.y - line_r2s.y)) /
                 h;
      float t2 = ((line_r1s.y - line_r1e.y) * (line_r1s.x - line_r2s.x) +
                  (line_r1e.x - line_r1s.x) * (line_r1s.y - line_r2s.y)) /
                 h;

      if (t1 >= 0.0f && t1 < 1.0f && t2 >= 0.0f && t2 < 1.0f) {
        Vector2 normal = Vector2Normalize(Vector2Subtract(pB, pA));
        Vector2 deltaSP = Vector2Subtract(rbB->speed, rbA->speed);
        float spAlongNormal = Vector2DotProduct(deltaSP, normal);
        if (spAlongNormal > 0)
          return true;

        float e = 0;
        float invMassA = (rbA->mass > 0) ? 1 / rbA->mass : 0;
        float invMassB = (rbB->mass > 0) ? 1 / rbB->mass : 0;
        float impulseMagnitude =
            -(1 + e) * spAlongNormal / (invMassA + invMassB);
        Vector2 impulse = Vector2Scale(normal, impulseMagnitude);
        rbA->speed =
            Vector2Subtract(rbA->speed, Vector2Scale(impulse, invMassA));
        rbB->speed = Vector2Add(rbB->speed, Vector2Scale(impulse, invMassB));
        return true;
      }
    }
  }
  return false;
}

void resolve_collision(Collision *input) {
  input->ta->position.x -= input->normal.x * input->distance;
  input->ta->position.y -= input->normal.y * input->distance;
}

void ecs_collision_system(Registry *r, Entity e) {
  Transform2 *ta = ecs_get(r, e, Transform2);
  Collider *ca = ecs_get(r, e, Collider);

  Signature mask =
      ((1 << ecs_cid(r, "Transform2") & (1 << ecs_cid(r, "Collider"))));
  Signature maskRB = (1 << ecs_cid(r, "RigidBody"));
  for (Entity other = e + 1; other < ecs_entity_count(r); ++other) {
    if (!ecs_has_component(r, e, mask))
      continue;
    Transform2 *tb = ecs_get(r, other, Transform2);
    Collider *cb = ecs_get(r, other, Collider);

    Collision collision;
    uint8_t overlap = 0;
    if (ca->solid && cb->solid) {
      int rb = false;
      RigidBody rbZero = {0};
      RigidBody *rbA, *rbB;
      rbA = rbB = &rbZero;
      if (ecs_has_component(r, e, maskRB)) {
        rbA = ecs_get(r, e, RigidBody);
        rb = true;
      }
      if (ecs_has_component(r, other, maskRB)) {
        rbB = ecs_get(r, other, RigidBody);
        rb = true;
      }

      if (!rb) {
        overlap = collision_sat(ta, ca, tb, cb, &collision);
        ca->overlap |= overlap;
        cb->overlap |= overlap;
      } else
        ca->overlap |=
            (collision_rb(ta->position, tb->position, ca, cb, rbA, rbB) ||
             collision_rb(ta->position, tb->position, ca, cb, rbA, rbB));
    } else {
      ca->overlap |= (collision_overlap(ta->position, ca, cb) ||
                      collision_overlap(tb->position, cb, ca));
    }

    if (overlap)
      resolve_collision(&collision);
  }
}
