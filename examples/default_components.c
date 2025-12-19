#include <ecs/component.h>
#include <ecs/system.h>

#include <stdio.h>

void MOVE(Registry *r, Entity e) {
  EntityData *ed = ecs_get(r, e, EntityData);
  printf("MOVING!!\n");
  ed->active = 0;
}

int main(void) {

  Registry *world = ecs_registry();

  ecs_component(world, EntityData);
  ecs_component(world, Behaviour);

  ecs_system(world, EcsOnStart, ecs_behaviour_system_start, Behaviour, EntityData);
  ecs_system(world, EcsOnUpdate, ecs_behaviour_system_update, Behaviour, EntityData);
  ecs_system(world, EcsOnLateUpdate, ecs_behaviour_system_late, Behaviour, EntityData);
  ecs_system(world, EcsOnFixedUpdate, ecs_behaviour_system_fixed, Behaviour, EntityData);
  ecs_system(world, EcsOnRender, ecs_behaviour_system_render, Behaviour, EntityData);
  ecs_system(world, EcsOnGui, ecs_behaviour_system_gui, Behaviour, EntityData);

  Entity e0 = ecs_entity(world);
  ecs_add(world, e0, Behaviour, {0});
  ecs_add(world, e0, EntityData, {1, 1});
  ecs_script(world, e0, MOVE, EcsOnUpdate);

  // SHOULD SHOW "MOVING!!" MESSAGE ONLY ONCE.
  ecs_run(world, EcsOnUpdate);
  ecs_run(world, EcsOnUpdate);

  return 0;
}
