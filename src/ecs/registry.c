#include <ecs/registry.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char *name;
  void *list;
  size_t size;
} ComponentList;

typedef struct {
  System *list;
  size_t size;
} LayerSystems;

struct ECS {
  Entity max_entities;
  Signature *entities;
  Entity entity_count;
  Entity *free_entities;
  Entity free_count;
  Entity max_free;

  ComponentList *components;
  Component comp_count;

  LayerSystems *systems;
};

void EcsAllocSystems(ECS *ecs);
void EcsFreeSystems(ECS *ecs);

void EcsAllocEntities(ECS *ecs, Entity max_entities);
void EcsFreeEntities(ECS *ecs);

void EcsFreeComponents(ECS *ecs);

ECS *EcsRegistry(uint16_t max_entities) {
  ECS *ecs = malloc(sizeof(ECS));
  ecs->max_free = 0;
  ecs->components = NULL;
  ecs->comp_count = 0;
  ecs->systems = NULL;
  EcsAllocEntities(ecs, max_entities);
  EcsAllocSystems(ecs);
  return ecs;
}

void EcsFree(ECS *ecs) {
  EcsFreeSystems(ecs);
  EcsFreeComponents(ecs);
  EcsFreeEntities(ecs);
  free(ecs);
}

// ######## //
//  ENTITY  //
// ######## //

void EcsAllocEntities(ECS *ecs, Entity max_entities) {
  ecs->max_entities = max_entities;
  ecs->entities = calloc(max_entities, sizeof(Signature));
  ecs->free_entities = calloc(max_entities, sizeof(Entity));
  ecs->entity_count = 0;
  ecs->free_count = 0;
}

void EcsFreeEntities(ECS *ecs) {
  free(ecs->entities);
  ecs->entity_count = 0;
  free(ecs->free_entities);
  ecs->free_count = 0;
}

Entity EcsEntity(ECS *ecs) {
  Entity e;
  if (ecs->free_count > 0)
    e = ecs->free_entities[--ecs->free_count];
  else
    e = ecs->entity_count++;
  ecs->entities[e] = 0;
  return e;
}

void EcsEntityDestroy(ECS *ecs, Entity e) {
  for (Component c = 0; c < ecs->comp_count; c++)
    EcsRemoveComponent(ecs, e, c);
  ecs->free_entities[ecs->free_count++] = e;
}

// non-object-oriented encapsulation
Entity EcsEntityCount(ECS *ecs) { return ecs->entity_count; }

// ########### //
//  COMPONENT  //
// ########### //

Component EcsRegisterComponent(ECS *ecs, char *name, size_t size) {
  Component id = ecs->comp_count++;
  if (id == 0)
    ecs->components = malloc(sizeof(ComponentList));
  else
    ecs->components =
        realloc(ecs->components, sizeof(ComponentList) * ecs->comp_count);

  ecs->components[id].name = name;
  ecs->components[id].size = size;
  ecs->components[id].list = calloc(ecs->max_entities, size);
  return id;
}

void EcsFreeComponents(ECS *ecs) {
  while (ecs->comp_count)
    free(ecs->components[--ecs->comp_count].list);
  free(ecs->components);
  ecs->comp_count = 0;
}

void EcsAddComponent(ECS *ecs, Entity e, Component id, void *data) {
  assert(id < ecs->comp_count && "Component does not exist!");
  size_t size = ecs->components[id].size;
  void *dest = ecs->components[id].list + e * size;
  memcpy(dest, data, size);
  ecs->entities[e] |= (1 << id);
}

void *EcsGetComponent(ECS *ecs, Entity e, Component id) {
  assert(id < ecs->comp_count && "Component does not exist!");
  assert(EcsHasComponent(ecs, e, (1 << id)) &&
         "Entity does not have the requiered component!");
  return ecs->components[id].list + e * ecs->components[id].size;
}

void *EcsGetComponentOptional(ECS *ecs, Entity e, Component id) {
  assert(id < ecs->comp_count && "Component does not exist!");
  if (!EcsHasComponent(ecs, e, (1 << id)))
    return NULL;
  return ecs->components[id].list + e * ecs->components[id].size;
}

void EcsRemoveComponent(ECS *ecs, Entity e, Component id) {
  assert(id < ecs->comp_count && "Component does not exist!");
  if (!EcsHasComponent(ecs, e, (1 << id)))
    return;
  size_t size = ecs->components[id].size;
  void *dest = ecs->components[id].list + e * size;
  memset(dest, 0, size);
  ecs->entities[e] &= ~(1 << id);
}

int EcsHasComponent(ECS *ecs, Entity e, Signature mask) {
  return (ecs->entities[e] & mask) == mask;
}

Component EcsCID(ECS *ecs, char *name) {
  for (Component id = 0; id < ecs->comp_count; id++) {
    if (strcmp(ecs->components[id].name, name) == 0)
      return id;
  }
  return ecs->comp_count;
}

// ######### //
//  SYSTEMS  //
// ######### //

void EcsAllocSystems(ECS *ecs) {
  ecs->systems = calloc(EcsSystemLayers, sizeof(LayerSystems));
}

void EcsFreeSystems(ECS *ecs) {
  for (int i = 0; i < EcsSystemLayers; i++)
    free(ecs->systems[i].list);
  free(ecs->systems);
}

void EcsAddSystem(ECS *ecs, Script s, EcsLayer ly, Signature mask) {
  size_t cur = ecs->systems[ly].size++;
  if (cur == 0)
    ecs->systems[ly].list = malloc(sizeof(System));
  else
    ecs->systems[ly].list =
        realloc(ecs->systems[ly].list, sizeof(System) * ecs->systems[ly].size);

  ecs->systems[ly].list[cur].run = s;
  ecs->systems[ly].list[cur].mask = mask;
}

uint8_t EcsCanRun(ECS *ecs, System *system, Entity e, EcsLayer ly) {
  if (!EcsHasComponent(ecs, e, system->mask))
    return 0;
  if (ly < EcsOnRender)
    return EcsEntityIsActive(ecs, e);
  else
    return EcsEntityIsVisible(ecs, e);
}

void EcsRun(ECS *ecs, EcsLayer ly) {
  size_t len = ecs->systems[ly].size;
  for (size_t s = 0; s < len; s++) {
    for (Entity e = 0; e < ecs->entity_count; e++) {
      if (EcsCanRun(ecs, &ecs->systems[ly].list[s], e, ly))
        ecs->systems[ly].list[s].run(ecs, e);
    }
  }
}
