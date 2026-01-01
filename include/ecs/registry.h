#ifndef ECS_REGISTRY_H
#define ECS_REGISTRY_H

#include <stddef.h>
#include <stdint.h>

typedef struct ECS ECS;
typedef uint16_t Entity;
typedef uint16_t Signature;
typedef uint16_t Component;
typedef void (*Script)(ECS *, Entity);
typedef struct {
  Script run;
  Signature mask;
} System;

ECS *EcsRegistry(uint16_t max_entities);
void EcsFree(ECS *ecs);

Entity EcsEntity(ECS *ecs);
void EcsEntityDestroy(ECS *ecs, Entity e);

void EcsEntitySetActive(ECS *ecs, Entity e, uint8_t active);
uint8_t EcsEntityIsActive(ECS *ecs, Entity e);

void EcsEntitySetVisible(ECS *ecs, Entity e, uint8_t visible);
uint8_t EcsEntityIsVisible(ECS *ecs, Entity e);

// non-object-oriented encapsulation
Entity EcsEntityCount(ECS *ecs);

// ########### //
//  COMPONENT  //
// ########### //

#define EcsComponent(registry, C)                                              \
  Component C##_ = EcsRegisterComponent(registry, #C, sizeof(C))

#define EcsAdd(registry, entity, C, ...)                                       \
  EcsAddComponent(registry, entity, EcsCID(registry, #C), &(C)__VA_ARGS__)

#define EcsAddLocal(registry, entity, C, ...)                                  \
  EcsAddComponent(registry, entity, C##_, &(C)__VA_ARGS__)

#define EcsAddExt(registry, entity, C, ...)                                    \
  EcsAddComponent(registry, entity, EcsCID(registry, #C), &__VA_ARGS__)

#define EcsGet(registry, entity, C)                                            \
  (C *)EcsGetComponent(registry, entity, EcsCID(registry, #C))

#define EcsGetOptional(registry, entity, C)                                    \
  (C *)EcsGetComponentOptional(registry, entity, EcsCID(registry, #C))

Component EcsRegisterComponent(ECS *ecs, char *name, size_t size);
void EcsAddComponent(ECS *ecs, Entity e, Component id, void *data);
void EcsRemoveComponent(ECS *ecs, Entity e, Component id);
void *EcsGetComponent(ECS *ecs, Entity e, Component id);
void *EcsGetComponentOptional(ECS *ecs, Entity e, Component id);

int EcsHasComponent(ECS *ecs, Entity e, Signature mask);
Component EcsCID(ECS *ecs, char *name);

// ######### //
//  SYSTEMS  //
// ######### //

#define ECS_FIXED_DELTATIME 1.f / 60.f

typedef enum {
  EcsOnStart = 0,
  EcsOnUpdate,
  EcsOnLateUpdate,
  EcsOnFixedUpdate,
  EcsOnRender,
  EcsOnGui,
  EcsSystemLayers
} EcsLayer;

#define GET_MACRO(_1, _2, _3, _4, NAME, ...) NAME

#define EcsSignatureLocal(C) (1ULL << C##_)
#define EcsSignature(ecs, C) (1ULL << EcsCID((ecs), #C))

#define FOR_EACH_l1(m, x) m(x)
#define FOR_EACH_l2(m, x, ...) m(x) | FOR_EACH_l1(m, __VA_ARGS__)
#define FOR_EACH_l3(m, x, ...) m(x) | FOR_EACH_l2(m, __VA_ARGS__)
#define FOR_EACH_l4(m, x, ...) m(x) | FOR_EACH_l3(m, __VA_ARGS__)

#define FOR_EACH_1(m, ecs, x) m(ecs, x)
#define FOR_EACH_2(m, ecs, x, ...) m(ecs, x) | FOR_EACH_1(m, ecs, __VA_ARGS__)
#define FOR_EACH_3(m, ecs, x, ...) m(ecs, x) | FOR_EACH_2(m, ecs, __VA_ARGS__)
#define FOR_EACH_4(m, ecs, x, ...) m(ecs, x) | FOR_EACH_3(m, ecs, __VA_ARGS__)

#define FOR_EACH_l(m, ...)                                                     \
  GET_MACRO(__VA_ARGS__, FOR_EACH_l4, FOR_EACH_l3, FOR_EACH_l2,                \
            FOR_EACH_l1)(m, __VA_ARGS__)

#define FOR_EACH(m, ecs, ...)                                                  \
  GET_MACRO(__VA_ARGS__, FOR_EACH_4, FOR_EACH_3, FOR_EACH_2,                   \
            FOR_EACH_1)(m, ecs, __VA_ARGS__)

#define EcsSystemLocal(ecs, script, layer, ...)                                \
  EcsAddSystem(ecs, script, layer, FOR_EACH_l(EcsSignatureLocal, __VA_ARGS__))

#define EcsSystem(ecs, script, layer, ...)                                     \
  EcsAddSystem(ecs, script, layer, FOR_EACH(EcsSignature, (ecs), __VA_ARGS__))

void EcsAddSystem(ECS *ecs, Script s, EcsLayer ly, Signature mask);
uint8_t EcsCanRun(ECS *ecs, System *system, Entity e, EcsLayer ly);
void EcsRun(ECS *ecs, EcsLayer ly);

#endif
