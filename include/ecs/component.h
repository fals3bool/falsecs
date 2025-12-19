#ifndef ECS_COMPONENT_H
#define ECS_COMPONENT_H

#include <ecs/registry.h>
#include <stdint.h>

typedef struct {
  uint8_t active;
  uint8_t visible;
} EntityData;

typedef struct {
  Script scripts[EcsSystemLayers];
  Script OnEnable;
  Script OnDisable;
  Script OnCollisionEnter;
} Behaviour;

void ecs_script(Registry *r, Entity e, Script s, EcsLayer ly);

#endif
