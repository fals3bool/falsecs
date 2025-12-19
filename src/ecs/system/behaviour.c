#include <ecs/component.h>
#include <ecs/system.h>

void bsy(Registry *r, Entity e, EcsLayer ly) {
  Behaviour *beh = ecs_get(r, e, Behaviour);
  if (beh->scripts[ly])
    beh->scripts[ly](r, e);
}

void ecs_behaviour_system_start(Registry *r, Entity e) {
  EntityData *ed = ecs_get(r, e, EntityData);
  if (!ed->active)
    return;
  bsy(r, e, EcsOnStart);
}
void ecs_behaviour_system_update(Registry *r, Entity e) {
  EntityData *ed = ecs_get(r, e, EntityData);
  if (!ed->active)
    return;
  bsy(r, e, EcsOnUpdate);
}
void ecs_behaviour_system_late(Registry *r, Entity e) {
  EntityData *ed = ecs_get(r, e, EntityData);
  if (!ed->active)
    return;
  bsy(r, e, EcsOnLateUpdate);
}
void ecs_behaviour_system_fixed(Registry *r, Entity e) {
  EntityData *ed = ecs_get(r, e, EntityData);
  if (!ed->active)
    return;
  bsy(r, e, EcsOnFixedUpdate);
}
void ecs_behaviour_system_render(Registry *r, Entity e) {
  EntityData *ed = ecs_get(r, e, EntityData);
  if (!ed->visible)
    return;
  bsy(r, e, EcsOnRender);
}
void ecs_behaviour_system_gui(Registry *r, Entity e) {
  EntityData *ed = ecs_get(r, e, EntityData);
  if (!ed->visible)
    return;
  bsy(r, e, EcsOnGui);
}
