#include <ecs/component.h>

void EntitySetActive(ECS *ecs, Entity e, uint8_t active) {
  EntityData *edata = EcsGet(ecs, e, EntityData);
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

uint8_t EntityIsActive(ECS *ecs, Entity e) {
  EntityData *edata = EcsGetOptional(ecs, e, EntityData);
  if (!edata)
    return true;
  return edata->active;
}

void EntityVisible(ECS *ecs, Entity e, uint8_t visible) {
  EntityData *edata = EcsGet(ecs, e, EntityData);
  edata->visible = visible;
}

uint8_t EntityIsVisible(ECS *ecs, Entity e) {
  EntityData *edata = EcsGetOptional(ecs, e, EntityData);
  if (!edata)
    return true;
  return edata->visible;
}
