#include <ecs/component.h>
#include <stdio.h>

typedef struct {
  float x;
  float y;
} Position;

int main(void) {

  Registry *r = ecs_registry();
  Entity e = ecs_entity(r);

  // - - - - -- - - - - //
  // REGISTER COMPONENT //
  // - - - - -- - - - - //

  // The registry stores a pair (string, int) with the name "Position" and the
  // Component ID. It also creates a local-scope variable called 'Position_'
  // that stores the Component ID.
  ecs_component(r, Position);

  // 'ecs_cid' retrieves the component id using the registry's pair (string,
  // int). then add a component using that id.
  Component c = ecs_cid(r, "Position");
  Position pos = {10, 10};
  ecs_add_component(r, e, c, &pos);
  // this functions uses memcpy, 'pos' can be changed without changing the
  // component itself.
  pos.x = 200;

  // Once the component is added, the registry stores a signature under the
  // entity id. It can be recreated using the component id.
  Signature s = (1 << ecs_cid(r, "Position"));

  // - - - - - - - - - - //
  // RETRIEVE COMPONENTS //
  // - - - - - - - - - - //

  // retrieve the component by its id.
  Position *p = ecs_get_component(r, e, c);
  printf("Position: {%.2f, %.2f}\n", p->x, p->y);

  // - - - - - - - - - - - - - - - - - - //
  // FALSECS DEFINES SOME USEFULL MACROS //
  // - - - - - - - - - - - - - - - - - - //

  // 'ecs_add' searches for the id using ecs_cid()
  ecs_add(r, e, Position, {20, 12});

  // 'ecs_add_local' uses the local-scope variable 'Position_'
  ecs_add_local(r, e, Position, {10, 20});

  // 'ecs_add_obj' adds a component by a defined struct.
  // 'ecs_add' always cast to 'Position' leading to an error in these cases.
  ecs_add_obj(r, e, Position, pos);

  // 'ecs_get' retrieves the component by its id using 'ecs_cid'.
  p = ecs_get(r, e, Position);
  printf("Position: {%.2f, %.2f}", p->x, p->y);

  return 0;
}
