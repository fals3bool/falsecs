#include <ecs/component.h>

void EntitySetActive(ECS *ecs, Entity e, uint8_t active) {
  EntityData *edata = EcsGetOptional(ecs, e, EntityData);
  if (edata) {
    uint8_t prev = edata->active;
    edata->active = active;
    Behaviour *beh = EcsGetOptional(ecs, e, Behaviour);
    if (beh && active != prev) {
      if (active) {
        if (beh->OnEnable)
          beh->OnEnable(ecs, e);
      } else if (beh->OnDisable)
        beh->OnDisable(ecs, e);
    }
  }

  Children *children = EcsGetOptional(ecs, e, Children);
  if (children)
    for (Entity i = 0; i < children->count; i++)
      EntitySetActive(ecs, children->list[i], active);
}

uint8_t EntityIsActive(ECS *ecs, Entity e) {
  EntityData *edata = EcsGetOptional(ecs, e, EntityData);
  if (edata)
    return edata->active;

  Parent *parent = EcsGetOptional(ecs, e, Parent);
  while (parent)
    return EntityIsActive(ecs, parent->entity);
  return true;
}

void EntitySetVisible(ECS *ecs, Entity e, uint8_t visible) {
  EntityData *edata = EcsGetOptional(ecs, e, EntityData);
  if (edata)
    edata->visible = visible;

  Children *children = EcsGetOptional(ecs, e, Children);
  if (children)
    for (Entity i = 0; i < children->count; i++)
      EntitySetVisible(ecs, children->list[i], visible);
}

uint8_t EntityIsVisible(ECS *ecs, Entity e) {
  EntityData *edata = EcsGetOptional(ecs, e, EntityData);
  if (edata)
    return edata->visible;

  Parent *parent = EcsGetOptional(ecs, e, Parent);
  while (parent)
    return EntityIsVisible(ecs, parent->entity);
  return true;
}
