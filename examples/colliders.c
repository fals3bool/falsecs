#include <scene/manager.h>

#include <raymath.h>

#define SCREEN_W 800
#define SCREEN_H 450
void load_own_entities(Scene *sc);

void script_move(Registry *r, Entity self) {
  Transform2 *t = ecs_get(r, self, Transform2);

  Camera2D *cam = ecs_get(r, 0, Camera2D);
  Vector2 mu = GetScreenToWorld2D(GetMousePosition(), *cam);
  Vector2 md = {mu.x - t->position.x, mu.y - t->position.y};
  t->rotation = atan2f(md.y, md.x);

  Vector2 d = {0};
  if (IsKeyDown(KEY_W))
    d.y -= 1;
  if (IsKeyDown(KEY_S))
    d.y += 1;
  if (IsKeyDown(KEY_A))
    d.x -= 1;
  if (IsKeyDown(KEY_D))
    d.x += 1;
  d = Vector2Scale(Vector2Normalize(d), GetFrameTime() * 90.f);
  t->position.x += d.x;
  t->position.y += d.y;
}

int main(void) {

  InitWindow(SCREEN_W, SCREEN_H, "scene manager usage");
  Camera2D cam = {{SCREEN_W / 2.f, SCREEN_H / 2.f}, {0, 0}, 0, 1.f};
  Color bg = {0, 0, 0, 255};

  FalsECS falsecs = falsecs_start(bg);
  Scene *sc = falsecs_scene(&falsecs, cam);

  load_own_entities(sc);

  falsecs_loop(&falsecs);
  falsecs_clean(&falsecs);
  CloseWindow();

  return 0;
}

void load_own_entities(Scene *sc) {
  Entity A = ecs_entity_wdata(sc);
  Entity B = ecs_entity_wdata(sc);
  ecs_add(sc, A, Transform2, TRANSFORM_POS(20, 20));
  ecs_add(sc, B, Transform2, TRANSFORM_POS(-20, -20));
  Collider col = collider_solid(3, 24);
  Collider co2 = collider_solid(5, 32);
  ecs_add_obj(sc, A, Collider, col);
  ecs_add_obj(sc, B, Collider, co2);
  ecs_add(sc, A, Behaviour, BEHAVIOUR_EMPTY);
  ecs_script(sc, A, script_move, EcsOnUpdate);
  ecs_system(sc, EcsOnRender, ecs_debug_collider_system, Transform2, Collider);
}
