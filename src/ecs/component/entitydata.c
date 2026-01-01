#include <ecs/component.h>

void EcsEntitySetActive(ECS *ecs, Entity e, uint8_t active) {
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

uint8_t EcsEntityIsActive(ECS *ecs, Entity e) {
  EntityData *edata = EcsGetOptional(ecs, e, EntityData);
  if (!edata)
    return true;
  return edata->active;
}

void EcsEntityVisible(ECS *ecs, Entity e, uint8_t visible) {
  EntityData *edata = EcsGet(ecs, e, EntityData);
  edata->visible = visible;
}

uint8_t EcsEntityIsVisible(ECS *ecs, Entity e) {
  EntityData *edata = EcsGetOptional(ecs, e, EntityData);
  if (!edata)
    return true;
  return edata->visible;
}
