#include <scene/manager.h>

void DRAW_SELF(Scene *scene, Entity self) {
  Transform2 *t = ecs_get(scene, self, Transform2);
  Rectangle rect = {t->position.x, t->position.y, 32, 32};
  DrawRectangleRec(rect, RED);
}

void MOVE(Scene *scene, Entity self) {
  Transform2 *t = ecs_get(scene, self, Transform2);
  t->position.x += 100 * GetFrameTime();
}

void load_test_scene(Scene *sc) {
  Entity e0 = falsecs_entity(sc);
  ecs_add_def(sc, e0, Transform2, TRANSFORM_DEFAULT);
  ecs_add_def(sc, e0, Behaviour, BEHAVIOUR_DEFAULT);
  ecs_script(sc, e0, MOVE, EcsOnUpdate);
  ecs_script(sc, e0, DRAW_SELF, EcsOnRender);
}

int main(void) {

  Color bg = {32, 33, 37, 255};
  FalsECS *falsecs = falsecs_start(800, 450, "raylib example", bg);
  Scene *sc = falsecs_scene(falsecs);

  load_test_scene(sc);
  falsecs_loop(falsecs);
  falsecs_free(falsecs);

  return 0;
}
