#ifndef ECS_COMPONENT_H
#define ECS_COMPONENT_H

#include <ecs/registry.h>
#include <raylib.h>

#include <math.h>
#include <stdint.h>

typedef struct {
  uint8_t active;
  uint8_t visible;
} EntityData;
#define ENTITYDATA_ACTIVE {1, 1}

typedef struct {
  Vector2 position;
  Vector2 scale;
  float rotation;
} Transform2;
#define TRANSFORM_ZERO {{0, 0}, {1, 1}, 0}
#define TRANSFORM_POS(x, y) {{x, y}, {1, 1}, 0}

typedef struct {
  Vector2 normal;
  float distance;
} Collision;

typedef struct {
  Entity self, other;
  Collision collision;
} CollisionEvent;

typedef struct {
  Vector2 *vx;
  Vector2 *md;
  uint8_t vertices;
  uint8_t overlap;
  uint8_t solid;
  void (*OnCollision)(CollisionEvent *event);
} Collider;

#define ColliderTrigger(v, r) ColliderCreate(v, r, 0)
#define ColliderSolid(v, r) ColliderCreate(v, r, 1)
Collider ColliderCreate(int vertices, float radius, uint8_t solid);

typedef enum {
  RIGIDBODY_STATIC = 0,
  RIGIDBODY_DYNAMIC,
  RIGIDBODY_KINEMATIC
} BodyType;

typedef struct {
  float mass;
  float invmass;
  float damping;
  BodyType type;
  uint8_t gravity;
  Vector2 speed;
  Vector2 acc;
} RigidBody;

#define rigidbody_create(mass, damping, type)                                  \
  {(mass > 0) ? mass : INFINITY,                                               \
   (mass > 0) ? 1.f / mass : 0,                                                \
   damping,                                                                    \
   type,                                                                       \
   (type == RIGIDBODY_DYNAMIC) ? 1 : 0,                                        \
   {0, 0},                                                                     \
   {0, 0}}

void BodyApplyForce(RigidBody *rb, Vector2 force);
void BodyApplyImpulse(RigidBody *rb, Vector2 impulse);
void BodyApplyDamping(RigidBody *rb);

typedef struct {
  Texture tex;
  Rectangle src;
  Color tint;
} Sprite;

typedef struct {
  Script OnEnable;
  Script OnDisable;
  Script OnCollisionEnter;
  Script scripts[EcsSystemLayers];
} Behaviour;
#define BEHAVIOUR_EMPTY {0}

void EcsScript(ECS *ecs, Entity e, Script s, EcsLayer ly);

#endif
