#ifndef ECS_REGISTRY_H
#define ECS_REGISTRY_H

#include <stddef.h>
#include <stdint.h>

typedef struct Registry Registry;

#define MAX_ENTITIES 1024
typedef uint16_t Entity;
typedef uint16_t Signature;
typedef uint16_t Component;
typedef void (*Script)(Registry *, Entity);
typedef struct {
  Script run;
  Signature mask;
} System;

Registry *ecs_registry();
void ecs_registry_free(Registry *r);

Entity ecs_entity(Registry *r);
void ecs_entity_destroy(Registry *r, Entity e);

// ########### //
//  COMPONENT  //
// ########### //

#define ecs_component(registry, C)                                             \
  Component C##_ = ecs_alloc_component(registry, #C, sizeof(C));

#define ecs_add(registry, entity, C, ...)                                      \
  ecs_add_component(registry, entity, C##_, &(C)__VA_ARGS__);

#define ecs_get(registry, entity, C)                                           \
  (C *)ecs_get_component(registry, entity, ecs_cid(registry, #C));

Component ecs_alloc_component(Registry *r, char *name, size_t size);
void ecs_add_component(Registry *r, Entity e, Component id, void *data);
void ecs_remove_component(Registry *r, Entity e, Component id);
void *ecs_get_component(Registry *r, Entity e, Component id);

int ecs_has_component(Registry *r, Entity e, Signature mask);
Component ecs_cid(Registry *r, char *name);

// ######### //
//  SYSTEMS  //
// ######### //

typedef enum {
  EcsOnStart = 0,
  EcsOnUpdate,
  EcsOnLateUpdate,
  EcsOnFixedUpdate,
  EcsOnRender,
  EcsOnGui,
  EcsSystemLayers
} EcsLayer;

void ecs_alloc_systems(Registry *r);
void ecs_alloc_system(Registry *r, EcsLayer ly, Script s, Signature mask);

void ecs_run(Registry *r, EcsLayer ly);

#endif
