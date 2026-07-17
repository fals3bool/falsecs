#include <ecs/component.h>

#include <assert.h>

void HierarchyDetach(ECS *ecs, Entity e) {
  Hierarchy *h = GetComponent(ecs, e, Hierarchy);
  if (!h || h->parent == InvalidID)
    return;

  if (h->leftSibling == InvalidID) { // parent
    Hierarchy *parent = GetComponent(ecs, h->parent, Hierarchy);
    assert(parent != NULL);
    parent->firstChild = h->rightSibling;
  } else { // left sibling
    Hierarchy *left = GetComponent(ecs, h->leftSibling, Hierarchy);
    left->rightSibling = h->rightSibling;
  }
  // right sibling
  if (h->rightSibling != InvalidID) {
    Hierarchy *right = GetComponent(ecs, h->rightSibling, Hierarchy);
    right->leftSibling = h->leftSibling;
  }

  h->parent = InvalidID;
  h->leftSibling = InvalidID;
  h->rightSibling = InvalidID;
}

bool HierarchyAttach(ECS *ecs, Entity parent, Entity child) {
  if (child == parent)
    return false;

  for (Entity p = parent; p != InvalidID;) {
    if (p == child)
      return false;

    Hierarchy *h = GetComponent(ecs, p, Hierarchy);
    if (!h)
      break;
    p = h->parent;
  }

  HierarchyDetach(ecs, child); // remove current parent

  Component comp = ComponentID(ecs, Hierarchy);

  if (!EcsHasComponent(ecs, parent, comp))
    AddComponent(ecs, parent, Hierarchy, {InvalidID, InvalidID, InvalidID, InvalidID});
  Hierarchy *p = GetComponent(ecs, parent, Hierarchy);

  if (!EcsHasComponent(ecs, child, comp))
    AddComponent(ecs, child, Hierarchy, {InvalidID, InvalidID, InvalidID, InvalidID});
  Hierarchy *c = GetComponent(ecs, child, Hierarchy);

  c->parent = parent;
  c->leftSibling = InvalidID;
  c->rightSibling = p->firstChild;

  if (p->firstChild != InvalidID) {
    Hierarchy *first = GetComponent(ecs, p->firstChild, Hierarchy);
    first->leftSibling = child;
  }

  p->firstChild = child;
  EntitySetActive(ecs, child, EntityIsActive(ecs, parent));
  EntitySetVisible(ecs, child, EntityIsVisible(ecs, parent));
  return true;
}

static inline Entity HierarchyNextSibling(ECS *ecs, Entity e) {
  Hierarchy *h = GetComponent(ecs, e, Hierarchy);
  return h ? h->rightSibling : InvalidID;
}

void EntityDestroy(ECS *ecs, Entity e) {
  HierarchyDetach(ecs, e);
  Hierarchy *h = GetComponent(ecs, e, Hierarchy);
  for (Entity c = h->firstChild; c != InvalidID;) {
    Entity next = HierarchyNextSibling(ecs, c);
    HierarchyDetach(ecs, c);
    c = next;
  }
  EcsEntityFree(ecs, e);
}

void EntityDestroyRecursive(ECS *ecs, Entity e) {
  HierarchyDetach(ecs, e);
  Hierarchy *h = GetComponent(ecs, e, Hierarchy);
  for (Entity c = h->firstChild; c != InvalidID;) {
    Entity next = HierarchyNextSibling(ecs, c);
    EntityDestroyRecursive(ecs, c);
    c = next;
  }
  EcsEntityFree(ecs, e);
}

void HierarchyForEachChild(ECS *ecs, Entity e, Script s) {
  Hierarchy *h = GetComponent(ecs, e, Hierarchy);
  for (Entity c = h->firstChild; c != InvalidID;) {
    Entity next = HierarchyNextSibling(ecs, c);
    s(ecs, c);
    c = next;
  }
}

void HierarchyForEachChildRecursive(ECS *ecs, Entity e, Script s) {
  Hierarchy *h = GetComponent(ecs, e, Hierarchy);
  for (Entity c = h->firstChild; c != InvalidID;) {
    Entity next = HierarchyNextSibling(ecs, c);
    s(ecs, c);
    HierarchyForEachChildRecursive(ecs, c, s);
    c = next;
  }
}

void EntitySetActive(ECS *ecs, Entity e, bool active) {
  EntitySetActiveSelf(ecs, e, active);
  Hierarchy *h = GetComponent(ecs, e, Hierarchy);
  for (Entity c = h->firstChild; c != InvalidID;) {
    EntitySetActive(ecs, c, active);
    c = HierarchyNextSibling(ecs, c);
  }
}

void EntitySetVisible(ECS *ecs, Entity e, bool visible) {
  EntitySetVisibleSelf(ecs, e, visible);
  Hierarchy *h = GetComponent(ecs, e, Hierarchy);
  for (Entity c = h->firstChild; c != InvalidID;) {
    EntitySetVisible(ecs, c, visible);
    c = HierarchyNextSibling(ecs, c);
  }
}
