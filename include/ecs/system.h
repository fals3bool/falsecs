#ifndef ECS_SYSTEM_H
#define ECS_SYSTEM_H

#include <ecs/registry.h>

void BehaviourStartSystem(ECS *ecs, Entity e);
void BehaviourUpdateSystem(ECS *ecs, Entity e);
void BehaviourLateSystem(ECS *ecs, Entity e);
void BehaviourFixedSystem(ECS *ecs, Entity e);
void BehaviourRenderSystem(ECS *ecs, Entity e);
void BehaviourGuiSystem(ECS *ecs, Entity e);

void HierarchyTransform(ECS *ecs, Entity e);
void TransformColliderSystem(ECS *ecs, Entity e);
void CollisionSystem(ECS *ecs, Entity e);
void DebugColliderSystem(ECS *ecs, Entity e);

void PhysicsSystem(ECS *ecs, Entity e);
void GravitySystem(ECS *ecs, Entity e);

void SpriteSystem(ECS *ecs, Entity e);

#endif
