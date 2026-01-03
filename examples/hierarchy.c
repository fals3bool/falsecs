#include <ecs/component.h>

#include <stdio.h>

void printHierarchy(ECS *ecs, Entity e) {
  Parent *parent = EcsGetOptional(ecs, e, Parent);
  Children *children = EcsGetOptional(ecs, e, Children);
  printf("Hierarchy Relations of [Entity: %d]\n", e);
  if (parent)
    printf("Parent -> %d\n", parent->entity);
  if (children) {
    printf("Children:\n");
    for (Entity e = 0; e < children->count; e++)
      printf(" -> %d\n", children->list[e]);
  }
  printf("\n");
}

int main(void) {

  ECS *ecs = EcsRegistry(32);
  EcsComponent(ecs, Parent);
  EcsComponent(ecs, Children);

  Entity A = EcsEntity(ecs);
  Entity B = EcsEntity(ecs);
  Entity C = EcsEntity(ecs);

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
  
  EntityDestroy(ecs, B);
  
  printHierarchy(ecs, A);
  // printHierarchy(ecs, B);
  printHierarchy(ecs, C);

  return 0;
}
