#include <ecs/component.h>
#include <stdio.h>

typedef struct {
  float x;
  float y;
} Position;

int main(void) {

  Registry *r = ecs_registry();

  // Register component
  ecs_component(r, Position);
  // The registry stores a pair (string, int) with the name "Position" and the
  // Component ID. It also creates a local-scope variable called Position_ that
  // stores the Component ID.

  Entity e = ecs_entity(r);

  // add the component using the var Position_
  // adding a component to an entity modifies its signature using the ID.
  ecs_add(r, e, Position, {20, 12});
  // calling ecs_add(...) outside this function will lead to an error:
  // err: Use of undeclared identifier 'Position_'.

  Position *p = ecs_get(r, e, Position);
  // by default ecs_get searches for ids inside the registry pair (string, int)
  // list.

  printf("Position: {%.2f, %.2f}", p->x, p->y);

  return 0;
}
