#include <assert.h>
#include <ecs/registry.h>

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

struct Registry {
  Signature entities[MAX_ENTITIES];
  Entity entity_count;
  Entity free_entities[MAX_ENTITIES];
  Entity free_count;

  ComponentList *components;
  Component comp_count;

  LayerSystems *systems;
};

void ecs_alloc_systems(Registry *r);
Registry *ecs_registry() {
  Registry *r = malloc(sizeof(Registry));
  r->entity_count = 0;
  r->free_count = 0;
  r->comp_count = 0;
  r->components = NULL;
  r->systems = NULL;
  ecs_alloc_systems(r);
  return r;
}

void ecs_registry_free(Registry *r) {
  while (r->comp_count)
    free(r->components[--r->comp_count].list);
  free(r->components);
  ecs_free_systems(r);
  free(r);
}

Entity ecs_entity(Registry *r) {
  Entity e;
  if (r->free_count > 0)
    e = r->free_entities[--r->free_count];
  else
    e = r->entity_count++;
  r->entities[e] = 0;
  return e;
}

void ecs_entity_destroy(Registry *r, Entity e) {
  for (Component c = 0; c < r->comp_count; c++)
    ecs_remove_component(r, e, c);
  r->free_entities[r->free_count++] = e;
}

// ########### //
//  COMPONENT  //
// ########### //

Component ecs_alloc_component(Registry *r, char *name, size_t size) {
  Component id = r->comp_count++;
  if (id == 0)
    r->components = malloc(sizeof(ComponentList));
  else
    r->components =
        realloc(r->components, sizeof(ComponentList) * r->comp_count);

  r->components[id].name = name;
  r->components[id].size = size;
  r->components[id].list = calloc(MAX_ENTITIES, size);
  return id;
}

void ecs_add_component(Registry *r, Entity e, Component id, void *data) {
  assert(id < r->comp_count && "Component does not exist!");
  size_t size = r->components[id].size;
  void *dest = r->components[id].list + e * size;
  memcpy(dest, data, size);
  r->entities[e] |= (1 << id);
}

void ecs_remove_component(Registry *r, Entity e, Component id) {
  assert(id < r->comp_count && "Component does not exist!");
  if ((r->entities[e] & (1 << id)) == 0)
    return;
  size_t size = r->components[id].size;
  void *dest = r->components[id].list + e * size;
  memset(dest, 0, size);
  r->entities[e] &= ~(1 << id);
}

void *ecs_get_component(Registry *r, Entity e, Component id) {
  assert(id < r->comp_count && "Component does not exist!");
  return r->components[id].list + e * r->components[id].size;
}

int ecs_has_component(Registry *r, Entity e, Signature mask) {
  return (r->entities[e] & mask) == mask;
}

Component ecs_cid(Registry *r, char *name) {
  for (Component id = 0; id < r->comp_count; id++) {
    if (strcmp(r->components[id].name, name) == 0)
      return id;
  }
  return r->comp_count;
}

// non-object-oriented encapsulation
Entity ecs_entity_count(Registry *r) { return r->entity_count; }

// ######### //
//  SYSTEMS  //
// ######### //

void ecs_alloc_systems(Registry *r) {
  r->systems = calloc(EcsSystemLayers, sizeof(LayerSystems));
}

void ecs_free_systems(Registry *r) {
  for (int i = 0; i < EcsSystemLayers; i++)
    free(r->systems[i].list);
  free(r->systems);
}

void ecs_alloc_system(Registry *r, EcsLayer ly, Script s, Signature mask) {
  size_t cur = r->systems[ly].size++;
  if (cur == 0)
    r->systems[ly].list = malloc(sizeof(System));
  else
    r->systems[ly].list =
        realloc(r->systems[ly].list, sizeof(System) * r->systems[ly].size);

  r->systems[ly].list[cur].run = s;
  r->systems[ly].list[cur].mask = mask;
}

void ecs_run(Registry *r, EcsLayer ly) {
  for (Entity e = 0; e < r->entity_count; e++) {
    size_t len = r->systems[ly].size;
    for (size_t i = 0; i < len; i++) {
      if (!ecs_has_component(r, e, r->systems[ly].list[i].mask))
        continue;
      r->systems[ly].list[i].run(r, e);
    }
  }
}
