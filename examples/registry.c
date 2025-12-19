#include <ecs/registry.h>

#include <stdio.h>

void print_entities(Entity e1, Entity e2, Entity e3) {
  printf("\nentities ids:\n [e1] {%d}\n [e2] {%d}\n [e3] {%d}\n", e1, e2, e3);
}

typedef struct {
  float x;
  float y;
} Position;

void MOVE(Registry *r, Entity e) {
  Position *pos = ecs_get(r, e, Position);
  pos->x += 100;
}

int main(void) {

  Registry *world = ecs_registry();
  Entity e0 = ecs_entity(world);

  Entity e1 = ecs_entity(world);
  Entity e2 = ecs_entity(world);
  print_entities(e0, e1, e2);
  ecs_entity_destroy(world, e1);
  Entity e3 = ecs_entity(world);
  e1 = ecs_entity(world);
  print_entities(e1, e2, e3);

  ecs_component(world, Position);
  ecs_add(world, e1, Position, {23, 24});

  ecs_alloc_system(world, EcsOnUpdate, MOVE, ecs_cid(world, "Position"));
  ecs_run(world, EcsOnUpdate);

  Position *pos = ecs_get(world, e1, Position);
  printf("\nposition: {%.2f, %.2f}\n", pos->x, pos->y);

  ecs_registry_free(world);

  return 0;
}
