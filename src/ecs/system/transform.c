#include <ecs/component.h>
#include <ecs/system.h>

void HierarchyTransformSystem(ECS *ecs, Entity e) {
  Parent *p = GetComponent(ecs, e, Parent);
  Transform2 *t = GetComponent(ecs, e, Transform2);
  Transform2 *tp = GetComponent(ecs, p->entity, Transform2);
  if (!tp)
    return;
  t->position = Vector2Add(tp->position, t->localPosition);
  t->scale =
      (Vector2){tp->scale.x * t->localScale.x, tp->scale.y * t->localScale.y};
  t->rotation = tp->rotation + t->localRotation;
}

void TransformColliderSystem(ECS *ecs, Entity e) {
  Transform2 *t = GetComponent(ecs, e, Transform2);
  Collider *c = GetComponent(ecs, e, Collider);

  float angle = t->rotation;
  for (uint8_t i = 0; i < c->vertices; i++) {
    c->vx[i].x =
        c->md[i].x * cosf(angle) - c->md[i].y * sinf(angle) + t->position.x;
    c->vx[i].y =
        c->md[i].x * sinf(angle) + c->md[i].y * cosf(angle) + t->position.y;
  }
  c->overlap = false;
}
