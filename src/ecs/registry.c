#include <ecs/registry.h>

#include <mem/array.h>

// vi :170

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MaxEntities 65355

typedef struct {
  Component id;
  char *name;
} ComponentID;

typedef struct {
  void *list;
  size_t size;
  void (*dtor)(void *);
  Component alloc;
} ComponentData;

typedef struct {
  System *list;
  EcsID size;
  EcsID alloc;
} PhaseSystem;

typedef struct {
  char *name;
  Signature mask;
} Layer;

struct Registry {
  EntityData *entities; // EntityData - GameObjects
  Entity entity_count;
  Entity entity_alloc;

  Entity *free_entities; // Free entities stack
  Entity free_count;
  Entity free_alloc;

  ComponentData *components; // Component matrix
  ComponentID *search;       // Component tree search (id+name)
  Component comp_count;
  Component comp_alloc;

  PhaseSystem *systems; // Systems with phases

  Layer *layers; // Layer stack (order + collision)
  EcsID layer_count;
  EcsID layer_alloc;
};

// ###### //
//  UTIL  //
// ###### //

void EcsLogStatus(ECS *ecs) {
  printf("ECS Registry: {\n  Entity: {\n");
  printf("    Alive: %u (alloc:%u)\n", ecs->entity_count, ecs->entity_alloc);
  printf("    Free: %u (alloc:%u)\n", ecs->free_count, ecs->free_alloc);
  printf("  },\n  Components: {\n");
  printf("    List: %u (alloc:%u) [\n", ecs->comp_count, ecs->comp_alloc);
  for (Component i = 0; i < ecs->comp_count; i++)
    printf("      {id: %u, name: %s},\n", ecs->search[i].id,
           ecs->search[i].name);
  printf("    ],\n  },\n  Systems: {\n    List: %d [\n", EcsTotalPhases);
  for (int i = 0; i < EcsTotalPhases; i++)
    printf("      {phase: %d, count: %u, alloc: %u},\n", i,
           ecs->systems[i].size, ecs->systems[i].alloc);
  printf("    ],\n  },\n  Layers: len:%u (alloc:%u) [\n", ecs->layer_count,
         ecs->layer_alloc);
  for (EcsID i = 0; i < ecs->layer_count; i++)
    printf("      {name: %s, mask: %lb},\n", ecs->layers[i].name,
           ecs->layers[i].mask);
  printf("    ],\n  },\n}\n");
}

// ############# //
//  INIT & FREE  //
// ############# //

static void EcsInitEntities(ECS *ecs) {
  ecs->entities = NULL;
  ecs->entity_count = 0;
  ecs->entity_alloc = 0;
  ecs->free_entities = NULL;
  ecs->free_count = 0;
  ecs->free_alloc = 0;
  ecs->layers = NULL;
  ecs->layer_count = 0;
  ecs->layer_alloc = 0;
}

static void EcsFreeEntities(ECS *ecs) {
  free(ecs->entities);
  ecs->entities = NULL;
  ecs->entity_count = 0;
  ecs->entity_alloc = 0;
  free(ecs->free_entities);
  ecs->free_entities = NULL;
  ecs->free_count = 0;
  ecs->free_alloc = 0;
  if (ecs->layers) {
    free(ecs->layers);
    ecs->layers = NULL;
    ecs->layer_count = 0;
    ecs->layer_alloc = 0;
  }
}

static void EcsInitComponents(ECS *ecs) {
  ecs->components = NULL;
  ecs->search = NULL;
  ecs->comp_alloc = 0;
  ecs->comp_count = 0;
}

void EcsFreeComponents(ECS *ecs) {
  while (ecs->comp_count > 0) {
    Component id = ecs->comp_count - 1;
    for (Entity e = 0; e < ecs->components[id].alloc && e < ecs->entity_count;
         ++e)
      EcsRemoveComponent(ecs, e, id);

    free(ecs->components[id].list);
    ecs->components[id].list = NULL;
    ecs->comp_count--;
  }
  free(ecs->components);
  free(ecs->search);
  ecs->components = NULL;
  ecs->comp_alloc = 0;
}

static void EcsInitSystems(ECS *ecs) {
  ecs->systems = calloc(EcsTotalPhases, sizeof(PhaseSystem));
}

void EcsFreeSystems(ECS *ecs) {
  for (int i = 0; i < EcsTotalPhases; i++)
    free(ecs->systems[i].list);
  free(ecs->systems);
  ecs->systems = NULL;
}

// ########## //
//  REGISTRY  //
// ########## //

ECS *EcsRegistry(void) {
  ECS *ecs = malloc(sizeof(ECS));
  EcsInitEntities(ecs);
  EcsInitComponents(ecs);
  EcsInitSystems(ecs);
  return ecs;
}

void EcsFree(ECS *ecs) {
  EcsFreeSystems(ecs);
  EcsFreeComponents(ecs);
  EcsFreeEntities(ecs);
  free(ecs);
  // printf("GEARECS: Registry freed successfully!\n");
}

// ######## //
//  ENTITY  //
// ######## //

Entity EcsEntity(ECS *ecs, char *tag) {
  Entity e;
  if (ecs->free_count > 0) {
    e = ecs->free_entities[--ecs->free_count];
  } else {
    if (ecs->entity_count >= MaxEntities)
      return InvalidID;
    e = ecs->entity_count++;
  }
  assert(e < MaxEntities && "Exceeded maximum number of entities");
  EntityData ed = {0, true, true, tag, 0};
  Entity alloc = MemPushBack((void **)&ecs->entities, ecs->entity_alloc, e, &ed,
                             sizeof(EntityData));
  // if (alloc == 0) // this should never happend
  //   return InvalidID;
  ecs->entity_alloc = alloc;
  return e;
}

bool EcsEntityIsAlive(ECS *ecs, Entity e) {
  for (Entity i = 0; i < ecs->free_count; i++)
    if (ecs->free_entities[i] == e)
      return false;
  return true;
}

void EcsEntityFree(ECS *ecs, Entity e) {
  // Remove all components with proper cleanup
  for (Component c = 0; c < ecs->comp_count; c++)
    EcsRemoveComponent(ecs, e, c);
  ecs->entities[e] = (EntityData){0};

  // if (ecs->free_count < MaxEntities) {
  Entity alloc = MemPushBack((void **)&ecs->free_entities, ecs->free_alloc,
                             ecs->free_count, &e, sizeof(Entity));
  ecs->free_alloc = alloc;
  ecs->free_count++;
  // }
}

Entity EcsEntityCount(ECS *ecs) { return ecs->entity_count - ecs->free_count; }

void EcsForEachEntity(ECS *ecs, Script script) {
  for (Entity e = 0; e < ecs->entity_count; e++) {
    if (!EcsEntityIsAlive(ecs, e))
      continue;
    script(ecs, e);
  }
}

// ######### //
//   STATE   //
// ######### //

EntityData *EcsEntityData(ECS *ecs, Entity e) {
  assert(e < MaxEntities && "Invalid entity");
  assert(e < ecs->entity_count && "Entity does not exist");
  return &ecs->entities[e];
}

Entity EntityFindByTag(ECS *ecs, char *tag) {
  for (Entity e = 0; e < ecs->entity_count; e++) {
    if (strcmp(ecs->entities[e].tag, tag) == 0)
      return e;
  }
  return InvalidID;
}

bool EntityHasTag(ECS *ecs, Entity e, char *tag) {
  assert(e < MaxEntities && "Invalid entity");
  assert(e < ecs->entity_count && "Entity does not exist");
  return strcmp(ecs->entities[e].tag, tag) == 0;
}

void EntitySetActiveSelf(ECS *ecs, Entity e, bool active) {
  assert(e < MaxEntities && "Invalid entity");
  assert(e < ecs->entity_count && "Entity does not exist");
  ecs->entities[e].active = active;
}

bool EntityIsActive(ECS *ecs, Entity e) {
  assert(e < MaxEntities && "Invalid entity");
  assert(e < ecs->entity_count && "Entity does not exist");
  return ecs->entities[e].active;
}

void EntitySetVisibleSelf(ECS *ecs, Entity e, bool visible) {
  assert(e < MaxEntities && "Invalid entity");
  assert(e < ecs->entity_count && "Entity does not exist");
  ecs->entities[e].visible = visible;
}

bool EntityIsVisible(ECS *ecs, Entity e) {
  assert(e < MaxEntities && "Invalid entity");
  assert(e < ecs->entity_count && "Entity does not exist");
  return ecs->entities[e].visible;
}

// ########### //
//  COMPONENT  //
// ########### //

int comp(const void *a, const void *b) {
  return strcmp(((ComponentID *)a)->name, ((ComponentID *)b)->name);
}

Component EcsComponent(ECS *ecs, char *name, size_t size,
                       void (*dtor)(void *)) {
  // maximum number of components for signatures
  if (ecs->comp_count >= 64)
    return InvalidID;

  Component id = ecs->comp_count;
  Component count = ecs->comp_count;
  Component alloc = ecs->comp_alloc;

  ComponentData component = {NULL, size, dtor, 0};
  ComponentID compid = {id, name};

  MemPushBack((void **)&ecs->components, alloc, count, &component,
              sizeof(ComponentData));
  alloc = MemPushBack((void **)&ecs->search, alloc, count, &compid,
                      sizeof(ComponentID));

  ecs->comp_alloc = alloc;
  ecs->comp_count++;

  qsort(ecs->search, ecs->comp_count, sizeof(ComponentID), comp);
  return id;
}

void EcsAddComponent(ECS *ecs, Entity e, Component id, void *data) {
  assert(e < MaxEntities && "Invalid entity");
  assert(id < 64 && "Invalid component");
  assert(e < ecs->entity_count && "Entity does not exist");
  assert(id < ecs->comp_count && "Component does not exist");

  size_t size = ecs->components[id].size;
  Component alloc = MemPushBack((void **)&ecs->components[id].list,
                                ecs->components[id].alloc, e, data, size);
  ecs->components[id].alloc = alloc;
  ecs->entities[e].signature |= (1ULL << id);
}

void *EcsGetComponent(ECS *ecs, Entity e, Component id) {
  if (!EcsHasComponent(ecs, e, id))
    return NULL;

  return (uint8_t *)ecs->components[id].list + e * ecs->components[id].size;
}

void EcsRemoveComponent(ECS *ecs, Entity e, Component id) {
  if (!EcsHasComponent(ecs, e, id))
    return;

  size_t size = ecs->components[id].size;
  void *dest = (uint8_t *)ecs->components[id].list + e * size;
  if (ecs->components[id].dtor)
    ecs->components[id].dtor(dest);
  memset(dest, 0, size);
  ecs->entities[e].signature &= ~(1ULL << id);
}

bool EcsHasComponent(ECS *ecs, Entity e, Component id) {
  assert(e < MaxEntities && "Invalid entity");
  assert(id < 64 && "Invalid component");
  assert(e < ecs->entity_count && "Entity does not exist");
  assert(id < ecs->comp_count && "Component does not exist");

  if ((ecs->entities[e].signature & (1ULL << id)) == 0)
    return false;
  return true;
}

bool EcsHasComponents(ECS *ecs, Entity e, Signature mask) {
  assert(e < MaxEntities && "Invalid entity");
  assert(e < ecs->entity_count && "Entity does not exist");

  return (ecs->entities[e].signature & mask) == mask;
}

Component EcsComponentID(ECS *ecs, char *name) {
  Component a = 0, b = ecs->comp_count - 1;
  while (a <= b) {
    Component k = (a + b) / 2;
    int c = strcmp(ecs->search[k].name, name);
    if (c == 0)
      return ecs->search[k].id;

    if (c > 0)
      b = k - 1;
    else
      a = k + 1;
  }
  return InvalidID;
}

// ########### //
//  SIGNATURE  //
// ########### //

size_t split(char *str, char **out, size_t max) {
  size_t count = 0;
  char *tok = strtok(str, ",");

  while (tok && count < max) {
    while (isspace((unsigned char)*tok))
      tok++;

    char *end = tok + strlen(tok) - 1;
    while (end > tok && isspace((unsigned char)*end)) {
      *end-- = '\0';
    }

    out[count++] = tok;
    tok = strtok(NULL, ",");
  }

  return count;
}

Signature EcsSignatureImpl(ECS *ecs, const char *str) {
  Signature mask = 0;

  char buffer[256];
  strncpy(buffer, str, sizeof(buffer));
  buffer[sizeof(buffer) - 1] = '\0';

  char *components[8];
  size_t n = split(buffer, components, 8);
  for (size_t i = 0; i < n; i++) {
    Component cid = EcsComponentID(ecs, components[i]);
    assert(cid < 64 && "Component not found"); // overflow signature bits
    mask |= (1ULL << cid);
  }
  return mask;
}

// ######### //
//  SYSTEMS  //
// ######### //

void EcsAddSystem(ECS *ecs, Script s, EcsPhase phase, Signature mask) {
  if (phase >= EcsTotalPhases)
    return;

  System sys = {s, mask};
  EcsID alloc =
      MemPushBack((void **)&ecs->systems[phase].list, ecs->systems[phase].alloc,
                  ecs->systems[phase].size, &sys, sizeof(System));
  if (alloc == InvalidID)
    return;

  ecs->systems[phase].alloc = alloc;
  ecs->systems[phase].size++;
}

void EcsRunSystems(ECS *ecs, EcsPhase phase) {
  size_t len = ecs->systems[phase].size;
  System *list = ecs->systems[phase].list;
  if (!list)
    return;

  for (size_t s = 0; s < len; s++) {
    for (Entity e = 0; e < ecs->entity_count; e++) {
      if (EcsHasComponents(ecs, e, list[s].mask) && EntityIsActive(ecs, e))
        list[s].run(ecs, e);
    }
  }
  return;
}

// ######## //
//  LAYERS  //
// ######## //

uint8_t LayerIndex(ECS *ecs, char *name) {
  for (int i = 0; i < ecs->layer_count; i++)
    if (strcmp(ecs->layers[i].name, name) == 0)
      return i;
  assert(!"Layer does not exist!");
  return ecs->layer_count;
}

void AddLayer(ECS *ecs, char *name) {
  EcsID count = ecs->layer_count;
  EcsID alloc = ecs->layer_alloc;

  Layer ly = {name, (Signature)-1}; // all enabled
  MemPushBack((void **)&ecs->layers, alloc, count, &ly, sizeof(Layer));

  ecs->layer_count++;
  ecs->layer_alloc = alloc;
}

void EcsFreeLayers(ECS *ecs) {
  if (ecs->layers)
    free(ecs->layers);
  ecs->layers = NULL;
  ecs->layer_count = 0;
}

void EntitySetLayer(ECS *ecs, Entity e, char *layer) {
  uint8_t ly = LayerIndex(ecs, layer);
  ecs->entities[e].layer = ly;
}

void LayerEnable(ECS *ecs, char *layer1, char *layer2) {
  uint8_t ly1 = LayerIndex(ecs, layer1);
  uint8_t ly2 = LayerIndex(ecs, layer2);
  ecs->layers[ly1].mask |= (1ULL << ly2);
  ecs->layers[ly2].mask |= (1ULL << ly1);
}

void LayerDisable(ECS *ecs, char *layer1, char *layer2) {
  uint8_t ly1 = LayerIndex(ecs, layer1);
  uint8_t ly2 = LayerIndex(ecs, layer2);
  ecs->layers[ly1].mask &= ~(1ULL << ly2);
  ecs->layers[ly2].mask &= ~(1ULL << ly1);
}

void LayerDisableAll(ECS *ecs, char *layer) {
  uint8_t ly = LayerIndex(ecs, layer);
  for (uint8_t l = 0; l < ecs->layer_count; l++)
    ecs->layers[l].mask &= ~(1ULL << ly);
  ecs->layers[ly].mask = 0;
}

bool LayerIncludes(ECS *ecs, uint8_t layer1, uint8_t layer2) {
  if (ecs->layer_count == 0)
    return true;
  Signature mask = (1ULL << layer2);
  return (ecs->layers[layer1].mask & mask) == mask;
}
