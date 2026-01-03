#include <ecs/component.h>
#include <ecs/system.h>

#include <stdio.h>

void printHierarchy(ECS *ecs, Entity e) {
  Parent *parent = EcsGetOptional(ecs, e, Parent);
  Children *children = EcsGetOptional(ecs, e, Children);
  printf("Hierarchy Relations of [Entity: %d] {%s}\n", e,
         EntityIsActive(ecs, e) ? "ACTIVE" : "NOT ACTIVE");
  if (parent)
    printf("Parent -> %d\n", parent->entity);
  if (children) {
    printf("Children:\n");
    for (Entity e = 0; e < children->count; e++)
      printf(" -> %d\n", children->list[e]);
  }

  Transform2 *t = EcsGetOptional(ecs, e, Transform2);
  if (t)
    printf("Position: {%.2f, %.2f}\n", t->position.x, t->position.y);

  printf("\n");
}

int main(void) {

  ECS *ecs = EcsRegistry(32);
  EcsComponent(ecs, Parent);
  EcsComponent(ecs, Children);
  EcsComponent(ecs, EntityData);
  EcsComponent(ecs, Transform2);

  EcsSystem(ecs, HierarchyTransform, 0, Transform2, Children);
  EcsAddSystem(ecs, printHierarchy, 0, 0);

  Entity A = EcsEntity(ecs);
  Entity B = EcsEntity(ecs);
  Entity C = EcsEntity(ecs);

  EcsAdd(ecs, A, EntityData, ENTITYDATA_ACTIVE);
  EcsAdd(ecs, B, EntityData, ENTITYDATA_ACTIVE);

  EcsAdd(ecs, A, Transform2, TRANSFORM_LOCALPOS(20, 30));
  EcsAdd(ecs, C, Transform2, TRANSFORM_POS(20, 30));

  EntityAddChild(ecs, B, A);
  EntityAddChild(ecs, B, C);

  EntityAddChild(ecs, A, C); // remove child from B, move to A
  EntityAddChild(ecs, C, C); // cannot
  EntityAddChild(ecs, C, A); // cannot
  EntityAddChild(ecs, C, B); // cannot

  EntityAddParent(ecs, C, A); // already done
  EntityAddParent(ecs, A, B); // already done
  EntityAddParent(ecs, A, C); // cannot
  EntityAddParent(ecs, C, B); // remove child from A, move to B
  EntityAddParent(ecs, A, C); // remove child from B, move to C
  EntityAddParent(ecs, B, A); // cannot

  EntitySetActive(ecs, C, false); // C will still be active because it does not
                                  // have EntityData and parent (B) is active
  EntitySetActive(ecs, B, true);  // All entities are now active

  EcsRun(ecs, 0);

  printf("Destroy B id:%d\n\n", B);
  EntityDestroy(ecs, B);          // destroy B, remove parent from C
  EntityDestroyRecursive(ecs, C); // destroy C and children...

  EcsRun(ecs, 0);

  return 0;
}
