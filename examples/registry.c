#include <ecs/registry.h>

#include <stdio.h>

#define PRINT_ID(entity) printf("%s {id:%d}\n", #entity, entity)

typedef struct {
  float x;
  float y;
} Position;

void MOVE(Registry *r, Entity e) {
  Position *pos = ecs_get(r, e, Position);
  pos->x += 100;
  printf("\nFrom system [MOVE] -> x += 100\n");
}

void PRINT(Registry *r, Entity e) {
  Position *pos = ecs_get(r, e, Position);
  printf("\nFrom system [PRINT] -> position: {%.2f, %.2f}\n", pos->x, pos->y);
}

int main(void) {

  // ECS REGISTRY
  Registry *world = ecs_registry();

  // ENTITIES
  Entity e0 = ecs_entity(world);
  PRINT_ID(e0);
  Entity e1 = ecs_entity(world);
  PRINT_ID(e1);
  Entity e2 = ecs_entity(world);
  PRINT_ID(e2);
  ecs_entity_destroy(world, e1);
  Entity e3 = ecs_entity(world);
  PRINT_ID(e3);
  e1 = ecs_entity(world);
  PRINT_ID(e1);

  // REGISTER AND ADD COMPONENT
  ecs_component(world, Position);
  ecs_add(world, e1, Position, {23, 24});

  // SYSTEMS USING THE COMPONENT 'Position'
  ecs_system(world, 0, MOVE, Position);
  ecs_system(world, 1, PRINT, Position);

  // RUN SYSTEMS UNDER EXECUTION LAYERS
  ecs_run(world, 1);
  ecs_run(world, 0);
  ecs_run(world, 1);

  ecs_registry_free(world);

  return 0;
}
