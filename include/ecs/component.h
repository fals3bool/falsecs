#ifndef ECS_COMPONENT_H
#define ECS_COMPONENT_H

#include <ecs/registry.h>
#include <raylib.h>

#include <stdint.h>

typedef struct {
  uint8_t active;
  uint8_t visible;
} EntityData;
#define ENTITYDATA_DEFAULT {1, 1}

typedef struct {
  Vector2 position;
  Vector2 scale;
  float rotation;
} Transform2;
#define TRANSFORM_DEFAULT {{0, 0}, {1, 1}, 0}

typedef struct {
  Texture tex;
  Rectangle src;
  Color tint;
} Sprite;

typedef struct {
  Script scripts[EcsSystemLayers];
  Script OnEnable;
  Script OnDisable;
  Script OnCollisionEnter;
} Behaviour;
#define BEHAVIOUR_DEFAULT {0}

void ecs_script(Registry *r, Entity e, Script s, EcsLayer ly);

#endif
