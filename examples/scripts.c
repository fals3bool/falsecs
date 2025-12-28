#include <scene/manager.h>

#define SCREEN_W 800
#define SCREEN_H 450

void move_self(Scene *sc, Entity self) {
  Transform2 *t = ecs_get(sc, self, Transform2);
  Vector2 m = {0};
  if (IsKeyDown(KEY_W))
    m.y -= 100 * GetFrameTime();
  if (IsKeyDown(KEY_S))
    m.y += 100 * GetFrameTime();
  if (IsKeyDown(KEY_A))
    m.x -= 100 * GetFrameTime();
  if (IsKeyDown(KEY_D))
    m.x += 100 * GetFrameTime();
  t->position.x += m.x;
  t->position.y += m.y;
}

void draw_self(Scene *sc, Entity self) {
  Transform2 *t = ecs_get(sc, self, Transform2);
  Rectangle rec = {t->position.x, t->position.y, 16, 16};
  DrawRectangleRec(rec, RED);
}

int main(void) {

  InitWindow(SCREEN_W, SCREEN_H, "Default Components");
  Camera2D cam = {{SCREEN_W / 2.f, SCREEN_H / 2.f}, {0, 0}, 0, 1.f};
  Color bg = {0, 0, 0, 255};

  FalsECS falsecs = falsecs_start(bg);
  Scene *sc = falsecs_scene(&falsecs, cam);

  Entity e = ecs_entity_wdata(sc);
  ecs_add(sc, e, Transform2, TRANSFORM_ZERO);

  // Behaviour is a component that let you add scripts.
  ecs_add(sc, e, Behaviour, {0});

  // Then add scripts under the dessired execution layer.
  ecs_script(sc, e, move_self, EcsOnUpdate);
  ecs_script(sc, e, draw_self, EcsOnRender);

  falsecs_loop(&falsecs);
  falsecs_clean(&falsecs);
  CloseWindow();

  return 0;
}
