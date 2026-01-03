#include <ecs/component.h>

#include <stdlib.h>

uint8_t EntitySetParent(ECS *ecs, Entity e, Entity p) {
  Parent *old = EcsGetOptional(ecs, e, Parent);
  if (old && old->entity == p)
    return false;

  if (old) {
    EntityRemoveChild(ecs, old->entity, e);
    old->entity = p;
  } else {
    EcsAdd(ecs, e, Parent, {p});
  }
  return true;
}

uint8_t EntitySetChild(ECS *ecs, Entity e, Entity c) {
  // E child of C child of E -> loop! (recursively)
  Parent *parent = EcsGetOptional(ecs, e, Parent);
  while (parent) {
    if (parent->entity == c)
      return false;
    parent = EcsGetOptional(ecs, parent->entity, Parent);
  }

  Children *children = EcsGetOptional(ecs, e, Children);
  if (children) {
    // Was already added?
    for (Entity i = 0; i < children->count; i++)
      if (children->list[i] == c)
        return false;

    Entity i = children->count++;
    if (children->count > children->allocated) {
      children->allocated = children->allocated ? children->allocated * 2 : 4;
    }
    children->list = realloc(children->list, sizeof(Entity) * children->count);
    children->list[i] = c;
  } else {
    Entity *list = malloc(sizeof(Entity));
    list[0] = c;
    EcsAdd(ecs, e, Children, {list, 1});
  }

  EntitySetActive(ecs, c, EntityIsActive(ecs, e));
  EntitySetVisible(ecs, c, EntityIsVisible(ecs, e));
  return true;
}

void EntityAddParent(ECS *ecs, Entity e, Entity p) {
  EntityAddChild(ecs, p, e);
}

void EntityAddChild(ECS *ecs, Entity e, Entity c) {
  if (e == c)
    return;
  if (EntitySetChild(ecs, e, c))
    EntitySetParent(ecs, c, e);
}

void EntityRemoveChild(ECS *ecs, Entity e, Entity c) {
  Children *children = EcsGetOptional(ecs, e, Children);
  if (!children || children->count <= 0)
    return;
  // replaced by last one
  // if count == 1 replaced by itself
  // ALWAYS count--; so id is never used until reasigned again.
  for (Entity i = 0; i < children->count; i++) {
    if (children->list[i] == c)
      children->list[i] = children->list[--children->count];
  }

  if (children->count == 0)
    EcsRemove(ecs, e, Children);
}

void EntityRemoveParent(ECS *ecs, Entity e) {
  Parent *parent = EcsGetOptional(ecs, e, Parent);
  if (!parent)
    return;
  EntityRemoveChild(ecs, e, parent->entity);
  EcsRemove(ecs, e, Parent);
}

void EntityDestroy(ECS *ecs, Entity e) {
  Parent *parent = EcsGetOptional(ecs, e, Parent);
  if (parent)
    EntityRemoveChild(ecs, parent->entity, e);

  Children *children = EcsGetOptional(ecs, e, Children);
  if (children)
    for (Entity i = 0; i < children->count; i++)
      EntityRemoveParent(ecs, children->list[i]);

  EcsEntityFree(ecs, e);
}
