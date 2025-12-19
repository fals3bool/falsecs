#include <assert.h>
#include <ecs/registry.h>

#include <stdlib.h>
#include <string.h>

typedef struct {
  char *name;
  void *list;
  size_t size;
} ComponentList;

struct Registry {
  Signature entities[MAX_ENTITIES];
  Entity entity_count;
  Entity free_entities[MAX_ENTITIES];
  Entity free_count;

  ComponentList *components;
  Component comp_count;
};

Registry *ecs_registry() {
  Registry *r = malloc(sizeof(Registry));
  r->entity_count = 0;
  r->free_count = 0;
  r->comp_count = 0;
  return r;
}

void ecs_registry_free(Registry *r) {
  while (r->comp_count)
    free(r->components[--r->comp_count].list);
  free(r->components);
  free(r);
}

Entity ecs_entity(Registry *r) {
  Entity e;
  if (r->free_count > 0)
    e = r->free_entities[--r->free_count];
  else
    e = r->entity_count++;
  return e;
}

void ecs_entity_destroy(Registry *r, Entity e) {
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
  size_t size = r->components[id].size;
  void *dest = r->components[id].list + e * size;
  memcpy(dest, data, size);
  r->entities[e] |= (1 << id);
}

void ecs_remove_component(Registry *r, Entity e, Component id) {
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
