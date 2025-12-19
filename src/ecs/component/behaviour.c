#include <ecs/component.h>

void ecs_script(Registry *r, Entity e, Script s, EcsLayer ly) {
  Behaviour *beh = ecs_get(r, e, Behaviour);
  beh->scripts[ly] = s;
}
