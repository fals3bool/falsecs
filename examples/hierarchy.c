#include <ecs/component.h>
#include <ecs/system.h>

#include <stdio.h>

void printID(ECS *ecs, Entity e) {
  char *tag = EcsEntityData(ecs, e)->tag;
  printf(" - [%d \"%s\"]\n", e, tag);
}

void printHierarchy(ECS *ecs, Entity e) {
  Parent *parent = GetComponent(ecs, e, Parent);
  Children *children = GetComponent(ecs, e, Children);
  printf("Hierarchy Relations of [Entity: %d] {%s} {%s}\n", e,
         EntityIsActive(ecs, e) ? "ACTIVE" : "NOT ACTIVE",
         EntityIsVisible(ecs, e) ? "VISIBLE" : "INVISIBLE");
  if (parent) {
    printf("Parent:\n");
    printID(ecs, parent->entity);
  }
  if (children) {
    printf("Children:\n");
    ForEachChild(ecs, e, printID);
  }

  Transform *t = GetComponent(ecs, e, Transform);
  if (t)
    printf("Position: {%.2f, %.2f, %.2f}\n", t->translation.x, t->translation.y,
           t->translation.z);

  printf("\n");
}

int main(void) {

  ECS *ecs = EcsRegistry();
  Component(ecs, Transform);
  Component(ecs, LocalTransform);
  Component(ecs, Parent);
  ComponentDynamic(ecs, Children, ChildrenDestructor);

  SystemGlobal(ecs, printHierarchy, 0);

  // foreach Child: Child.position = Parent.position + Child.localPosition
  System(ecs, HierarchyTransformSystem, 0, Transform, Parent);

  Entity A = EcsEntity(ecs, "A");
  Entity B = EcsEntity(ecs, "B");
  Entity C = EcsEntity(ecs, "C");

  AddComponent(ecs, A, Transform, TransformOrigin);
  AddComponent(ecs, A, LocalTransform, TransformPosition(20, 30, 10));
  AddComponent(ecs, C, Transform, TransformPosition(20, 30, 10));

  // Entity f = FindByTag(ecs, "B");
  // printf("found: {id: %d}\n", f);
  // f = FindByTag(ecs, "A");
  // printf("found: {id: %d}\n", f);

  AddChild(ecs, B, A);
  AddChild(ecs, B, C);

  EcsRunSystems(ecs, 0);

  printf("¬-¬-¬-¬-¬-¬-¬-¬-¬-\nHierarchy changed\n¬-¬-¬-¬-¬-¬-¬-¬-¬-\n\n");

  // The API will refuse to add a parent or child to prevent errors, like loops or duplications

  AddChild(ecs, A, C); // remove child from B, move to A
  AddChild(ecs, C, C); // cannot
  AddChild(ecs, C, A); // cannot
  AddChild(ecs, C, B); // cannot

  AddParent(ecs, C, A); // already done
  AddParent(ecs, A, B); // already done
  AddParent(ecs, A, C); // cannot
  AddParent(ecs, C, B); // remove child from A, move to B
  AddParent(ecs, A, C); // remove child from B, move to C
  AddParent(ecs, B, A); // cannot

  SetActive(ecs, C, false);  // deactivate C its children (recursively)
  SetVisible(ecs, C, false); // hide C and its children (recursively)

  // Deactivated entities won't be read by systems
  SetActive(ecs, B, true); // activate B and its children (recursively)

  EcsRunSystems(ecs, 0);

  printf("¬-¬-¬-¬-¬-¬-¬-\nDestroy B id:%d\n¬-¬-¬-¬-¬-¬-¬-\n\n", B);
  Destroy(ecs, B); // destroy B, remove parent from C
  // EntityDestroyRecursive(ecs, C); // destroy C and children...

  EntitySetVisible(ecs, C, true); // ignore children
  EcsRunSystems(ecs, 0);

  return 0;
}
