#ifndef ECS_SYSTEM_H
#define ECS_SYSTEM_H

#include <ecs/registry.h>

void ecs_behaviour_system_start(Registry *r, Entity e);
void ecs_behaviour_system_update(Registry *r, Entity e);
void ecs_behaviour_system_late(Registry *r, Entity e);
void ecs_behaviour_system_fixed(Registry *r, Entity e);
void ecs_behaviour_system_render(Registry *r, Entity e);
void ecs_behaviour_system_gui(Registry *r, Entity e);

void ecs_transform_collider_system(Registry *r, Entity e);
void ecs_collision_system(Registry *r, Entity e);
void ecs_collision_overlap_system(Registry *r, Entity e);

void ecs_gravity_system(Registry *r, Entity e);
void ecs_physics_system(Registry *r, Entity e);

void ecs_sprite_system(Registry *r, Entity e);
void ecs_debug_collider_system(Registry *r, Entity e);

#endif
