#include <ecs/component.h>
#include <ecs/system.h>

#include <stdio.h>

void printID(ECS *ecs, Entity e) {
  char *tag = EcsEntityData(ecs, e)->tag;
  printf(" - [%d \"%s\"]\n", e, tag);
}

void printHierarchy(ECS *ecs, Entity e) {
  Hierarchy *h = GetComponent(ecs, e, Hierarchy);
  printf("Hierarchy Relations of [Entity: %d] {%s} {%s}\n", e,
         EntityIsActive(ecs, e) ? "ACTIVE" : "NOT ACTIVE",
         EntityIsVisible(ecs, e) ? "VISIBLE" : "INVISIBLE");
  if (h) {
    printf("Parent:\n");
    if (h->parent != InvalidID)
      printID(ecs, h->parent);
    printf("Children:\n");
    HierarchyForEachChild(ecs, e, printID);
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
  Component(ecs, Hierarchy);

  SystemGlobal(ecs, printHierarchy, 0);

  // foreach Child: Child.position = Parent.position + Child.localPosition
  System(ecs, HierarchyTransformSystem, 0, Transform, Hierarchy);

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

  HierarchyAttach(ecs, B, A);
  HierarchyAttach(ecs, B, C);

  EcsRunSystems(ecs, 0);

  printf("¬-¬-¬-¬-¬-¬-¬-¬-¬-\nHierarchy changed\n¬-¬-¬-¬-¬-¬-¬-¬-¬-\n\n");

  // The API will refuse to add a parent or child to prevent errors, like loops
  // or duplications

  printf("%d\n", HierarchyAttach(ecs, A, C)); // remove child C from B, move to A
  printf("%d\n", HierarchyAttach(ecs, C, C)); // cannot
  printf("%d\n", HierarchyAttach(ecs, C, A)); // cannot
  printf("%d\n", HierarchyAttach(ecs, C, B)); // cannot

  printf("%d\n", HierarchyAttach(ecs, A, C)); // already done
  printf("%d\n", HierarchyAttach(ecs, B, A)); // already done
  printf("%d\n", HierarchyAttach(ecs, C, A)); // cannot
  printf("%d\n", HierarchyAttach(ecs, B, C)); // remove child C from A, move to B
  printf("%d\n", HierarchyAttach(ecs, C, A)); // remove child A from B, move to C
  printf("%d\n", HierarchyAttach(ecs, A, B)); // cannot

  EntitySetActive(ecs, C, false);  // deactivate C its children (recursively)
  EntitySetVisible(ecs, C, false); // hide C and its children (recursively)

  // Deactivated entities won't be read by systems
  EntitySetActive(ecs, B, true); // activate B and its children (recursively)

  EcsRunSystems(ecs, 0);

  printf("¬-¬-¬-¬-¬-¬-¬-\nDestroy B id:%d\n¬-¬-¬-¬-¬-¬-¬-\n\n", B);
  EntityDestroy(ecs, B); // destroy B, remove parent from C
  // EntityDestroyRecursive(ecs, C); // destroy C and children...

  EntitySetVisibleSelf(ecs, C, true); // ignore children
  EcsRunSystems(ecs, 0);

  return 0;
}
