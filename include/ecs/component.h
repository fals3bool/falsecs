#ifndef ECS_COMPONENT_H
#define ECS_COMPONENT_H

#include <ecs/registry.h>
#include <raylib.h>

#include <stdint.h>

#define VEC2ZERO (Vector2){0, 0}

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
  Vector2 *vx;
  Vector2 *md;
  Vector2 origin;
  float rot;
  uint8_t vertices;
  uint8_t overlap;
  uint8_t solid;
} Collider;

#define ecs_collider(v, r) ecs_collider_ex(v, r, 0, VEC2ZERO, 0)
#define ecs_collider_rot(v, r, a) ecs_collider_ex(v, r, a, VEC2ZERO, 0)
#define ecs_collider_offset(v, r, a, o) ecs_collider_ex(v, r, a, o, 0)
#define ecs_collider_solid(v, r) ecs_collider_ex(v, r, 0, VEC2ZERO, 1)
#define ecs_collider_rot_solid(v, r, a) ecs_collider_ex(v, r, a, VEC2ZERO, 1)
#define ecs_collider_offset_solid(v, r, a, o) ecs_collider_ex(v, r, a, o, 1)

Collider ecs_collider_ex(int vertices, float radius, float rot, Vector2 origin,
                         uint8_t solid);

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
