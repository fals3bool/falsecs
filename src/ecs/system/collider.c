#include <ecs/component.h>
#include <ecs/system.h>

#include <math.h>
#include <raymath.h>

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

uint8_t collision_sat(Transform2 *ta, Collider *a, Transform2 *tb,
                      Collider *b) {
  float overlap = INFINITY;

  for (uint8_t i = 0; i < a->vertices; i++) {
    uint8_t j = (i + 1) % a->vertices;
    Vector2 proj = {-(a->vx[j].y - a->vx[i].y), a->vx[j].x - a->vx[i].x};
    proj = Vector2Normalize(proj);

    float min_r1 = INFINITY, max_r1 = -INFINITY;
    for (uint8_t p = 0; p < a->vertices; p++) {
      float q = (a->vx[p].x * proj.x + a->vx[p].y * proj.y);
      min_r1 = fminf(min_r1, q);
      max_r1 = fmaxf(max_r1, q);
    }

    float min_r2 = INFINITY, max_r2 = -INFINITY;
    for (uint8_t p = 0; p < b->vertices; p++) {
      float q = (b->vx[p].x * proj.x + b->vx[p].y * proj.y);
      min_r2 = fminf(min_r2, q);
      max_r2 = fmaxf(max_r2, q);
    }

    overlap = fminf(fminf(max_r1, max_r2) - fmaxf(min_r1, min_r2), overlap);

    if (!(max_r2 >= min_r1 && max_r1 >= min_r2))
      return false;
  }

  Vector2 dis = {tb->position.x - ta->position.x,
                 tb->position.y - ta->position.y};
  dis = Vector2Normalize(dis);
  dis.x *= overlap;
  dis.y *= overlap;
  ta->position.x -= dis.x;
  ta->position.y -= dis.y;
  return false;
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

void ecs_collision_system(Registry *r, Entity e) {
  Transform2 *ta = ecs_get(r, e, Transform2);
  Collider *ca = ecs_get(r, e, Collider);

  Signature mask =
      ((1 << ecs_cid(r, "Transform2") & (1 << ecs_cid(r, "Collider"))));
  for (Entity other = e + 1; other < ecs_entity_count(r); ++other) {
    if (!ecs_has_component(r, e, mask))
      continue;
    Transform2 *tb = ecs_get(r, other, Transform2);
    Collider *cb = ecs_get(r, other, Collider);

    if (ca->solid && cb->solid)
      ca->overlap |= collision_sat(ta, ca, tb, cb);
    else
      ca->overlap |= (collision_overlap(ta->position, ca, cb) ||
                      collision_overlap(tb->position, cb, ca));
  }
}
