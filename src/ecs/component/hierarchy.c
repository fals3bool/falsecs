#include <ecs/component.h>

#include <stdlib.h>

// Component cleanup function
void ChildrenDestructor(void *self) {
  Children *children = (Children *)self;
  if (children && children->list) {
    free(children->list);
    children->list = NULL;
    children->count = 0;
    children->allocated = 0;
  }
}

// Every parent operation will call the children operation with swapped entities
// and change the parent component.
//
// The children operation is the one with the logic.

bool SetParent(ECS *ecs, Entity e, Entity p) {
  Parent *old = GetComponent(ecs, e, Parent);
  if (old && old->entity == p)
    return true;

  if (old)
    RemoveChild(ecs, old->entity, e);
  AddComponent(ecs, e, Parent, {p});
  return true;
}

bool SetChild(ECS *ecs, Entity e, Entity c) {
  // E child of C child of E -> loop! (recursively)
  Parent *parent = GetComponent(ecs, e, Parent);
  while (parent) {
    if (parent->entity == c)
      return false;
    parent = GetComponent(ecs, parent->entity, Parent);
  }

  Children *children = GetComponent(ecs, e, Children);
  if (children) {
    // Was already added?
    for (Entity i = 0; i < children->count; i++)
      if (children->list[i] == c)
        return false;

    // add to existing component, realloc if necessary
    if (children->count >= children->allocated) {
      size_t new_allocated = children->allocated ? children->allocated * 2 : 4;
      Entity *new_list =
          realloc(children->list, sizeof(Entity) * new_allocated);
      if (!new_list) {
        return false; // Reallocation failed
      }
      children->list = new_list;
      children->allocated = new_allocated;
    }
    children->list[children->count++] = c;
  } else {
    // add within a new component instance
    Entity *list = malloc(sizeof(Entity));
    if (!list)
      return false;
    list[0] = c;
    AddComponent(ecs, e, Children, {list, 1, 1});
  }

  // Hierarchical entitydata states
  EntitySetActive(ecs, c, EntityIsActive(ecs, e));
  EntitySetVisible(ecs, c, EntityIsVisible(ecs, e));
  return true;
}

void AddParent(ECS *ecs, Entity e, Entity p) { AddChild(ecs, p, e); }

void AddChild(ECS *ecs, Entity e, Entity c) {
  if (e == c)
    return;
  if (SetChild(ecs, e, c))
    SetParent(ecs, c, e);
}

void RemoveParent(ECS *ecs, Entity e) {
  Parent *parent = GetComponent(ecs, e, Parent);
  if (parent)
    RemoveChild(ecs, parent->entity, e);
}

void RemoveChild(ECS *ecs, Entity e, Entity c) {
  Children *children = GetComponent(ecs, e, Children);
  if (!children || children->count <= 0)
    return;
  // replaced by last one
  // if count == 1 replaced by itself
  // ALWAYS count--; so id is never used until reasigned again.
  for (Entity i = 0; i < children->count; i++) {
    if (children->list[i] == c)
      children->list[i] = children->list[--children->count];
  }

  RemoveComponent(ecs, c, Parent);
  if (children->count == 0)
    RemoveComponent(ecs, e, Children);
}

void Destroy(ECS *ecs, Entity e) {
  RemoveParent(ecs, e);

  Children *children = GetComponent(ecs, e, Children);
  if (children) {
    for (Entity i = 0; i < children->count; i++)
      RemoveParent(ecs, children->list[i]);
  }

  EcsEntityFree(ecs, e);
}

void DestroyRecursive(ECS *ecs, Entity e) {
  RemoveParent(ecs, e);

  Children *children = GetComponent(ecs, e, Children);
  if (children) {
    for (Entity i = 0; i < children->count; i++)
      DestroyRecursive(ecs, children->list[i]);
  }

  EcsEntityFree(ecs, e);
}

void ForEachChild(ECS *ecs, Entity e, Script s) {
  Children *children = GetComponent(ecs, e, Children);
  if (children) {
    for (Entity i = 0; i < children->count; i++)
      s(ecs, children->list[i]);
  }
}

void ForEachChildRecursive(ECS *ecs, Entity e, Script s) {
  Children *children = GetComponent(ecs, e, Children);
  if (children) {
    for (Entity i = 0; i < children->count; i++) {
      s(ecs, children->list[i]);
      ForEachChildRecursive(ecs, children->list[i], s);
    }
  }
}

void EntitySetActive(ECS *ecs, Entity e, bool active) {
  EntitySetActiveSelf(ecs, e, active);
  Children *children = GetComponent(ecs, e, Children);
  if (children) {
    for (Entity i = 0; i < children->count; i++)
      EntitySetActive(ecs, children->list[i], active);
  }
}

void EntitySetVisible(ECS *ecs, Entity e, bool visible) {
  EntitySetVisibleSelf(ecs, e, visible);
  Children *children = GetComponent(ecs, e, Children);
  if (children) {
    for (Entity i = 0; i < children->count; i++)
      EntitySetVisible(ecs, children->list[i], visible);
  }
}
