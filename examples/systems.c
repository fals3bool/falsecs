#include <ecs/system.h>
#include <stdio.h>

typedef struct {
  float x;
  float y;
} Position;

typedef struct {
  float x;
  float y;
} StaticPosition;

void move_system(Registry *r, Entity e) {
  Position *p = ecs_get(r, e, Position);
  p->x += 100;
  printf("entity id:{%d} moved x+=100\n", e);
}

void all_comps(Registry *r, Entity e) {
  printf("the entity id:{%d} has both components\n", e);
}

int main(void) {
  Registry *r = ecs_registry();
  ecs_component(r, Position);
  ecs_component(r, StaticPosition);

  // Register a system under a execution layer.
  // You also need to specify the component(s) to compare signatures.
  // This system runs for every entity that has a Position component.
  ecs_system(r, EcsOnUpdate, move_system, Position);
  // 'ecs_system_local' uses the local-scope variable 'Position_'
  // and 'StaticPosition_'
  ecs_system_local(r, EcsOnUpdate, all_comps, Position, StaticPosition);

  Entity e1 = ecs_entity(r);
  Entity e2 = ecs_entity(r);
  Entity e3 = ecs_entity(r);
  ecs_add_local(r, e1, Position, {20, 20});
  ecs_add_local(r, e2, StaticPosition, {20, 20});
  ecs_add_local(r, e3, Position, {20, 20});
  ecs_add_local(r, e3, StaticPosition, {20, 20});

  Position *p1 = ecs_get(r, e1, Position);
  StaticPosition *p2 = ecs_get(r, e2, StaticPosition);
  Position *p3 = ecs_get(r, e3, Position);
  printf("position 1: {%.2f, %.2f}\n", p1->x, p1->y);
  printf("position 2: {%.2f, %.2f}\n", p2->x, p2->y);
  printf("position 3: {%.2f, %.2f}\n\n", p3->x, p3->y);

  // Run systems:
  ecs_run(r, EcsOnUpdate);

  printf("position 1: {%.2f, %.2f}\n", p1->x, p1->y);
  printf("position 2: {%.2f, %.2f}\n", p2->x, p2->y);
  printf("position 3: {%.2f, %.2f}\n", p3->x, p3->y);

  ecs_registry_free(r);

  return 0;
}
