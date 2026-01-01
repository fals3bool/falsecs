#include <ecs/registry.h>

#include <stdio.h>

#define PRINT_ID(entity) printf("%s {id:%d}\n", #entity, entity)

typedef struct {
  float x;
  float y;
} Position;

void Move(ECS *ecs, Entity e) {
  Position *pos = EcsGet(ecs, e, Position);
  pos->x += 100;
  printf("\nFrom system [MOVE] -> x += 100\n");
}

void Print(ECS *ecs, Entity e) {
  Position *pos = EcsGet(ecs, e, Position);
  printf("\nFrom system [PRINT] -> position: {%.2f, %.2f}\n", pos->x, pos->y);
}

int main(void) {

  // ECS REGISTRY
  ECS *world = EcsRegistry(32);

  // ENTITIES
  Entity e0 = EcsEntity(world);
  PRINT_ID(e0);
  Entity e1 = EcsEntity(world);
  PRINT_ID(e1);
  Entity e2 = EcsEntity(world);
  PRINT_ID(e2);
  EcsEntityDestroy(world, e1);
  Entity e3 = EcsEntity(world);
  PRINT_ID(e3);
  e1 = EcsEntity(world);
  PRINT_ID(e1);

  // REGISTER AND ADD COMPONENT
  EcsComponent(world, Position);
  EcsAdd(world, e1, Position, {23, 24});

  // SYSTEMS USING THE COMPONENT 'Position'
  EcsSystem(world, Move, 0, Position);
  EcsSystem(world, Print, 1, Position);

  // RUN SYSTEMS UNDER EXECUTION LAYERS
  EcsRun(world, 1);
  EcsRun(world, 0);
  EcsRun(world, 1);

  EcsFree(world);

  return 0;
}
