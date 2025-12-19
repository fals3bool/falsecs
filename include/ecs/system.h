#ifndef ECS_SYSTEM_H
#define ECS_SYSTEM_H

#include <ecs/registry.h>

void ecs_behaviour_system_start(Registry *r, Entity e);
void ecs_behaviour_system_update(Registry *r, Entity e);
void ecs_behaviour_system_late(Registry *r, Entity e);
void ecs_behaviour_system_fixed(Registry *r, Entity e);
void ecs_behaviour_system_render(Registry *r, Entity e);
void ecs_behaviour_system_gui(Registry *r, Entity e);

#endif
