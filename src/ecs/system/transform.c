#include <ecs/component.h>
#include <ecs/system.h>

#include <raymath.h>

void HierarchyTransform(ECS *ecs, Entity e) {
  Children *children = EcsGet(ecs, e, Children);
  Transform2 *t = EcsGet(ecs, e, Transform2);

  for (Entity i = 0; i < children->count; i++) {
    Transform2 *ti = EcsGetOptional(ecs, children->list[i], Transform2);
    if (!ti)
      continue;
    ti->position = Vector2Add(t->position, ti->localPosition);
    ti->scale =
        (Vector2){t->scale.x * ti->localScale.x, t->scale.y * ti->localScale.y};
    ti->rotation = t->rotation + ti->localRotation;
  }
}
