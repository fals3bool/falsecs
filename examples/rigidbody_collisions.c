#include <scene/manager.h>

#include <raymath.h>
#include <stdio.h>

#define SCREEN_W 800
#define SCREEN_H 450

void move_self(Scene *sc, Entity self) {
  Transform2 *t = ecs_get(sc, self, Transform2);
  RigidBody *rb = ecs_get(sc, self, RigidBody);

  Vector2 d = {0};
  if (IsKeyDown(KEY_W))
    d.y -= 1;
  if (IsKeyDown(KEY_S))
    d.y += 1;
  if (IsKeyDown(KEY_A))
    d.x -= 1;
  if (IsKeyDown(KEY_D))
    d.x += 1;

  rb_apply_impulse(rb, Vector2Scale(Vector2Normalize(d), 400.f));
}

void draw_self(Scene *sc, Entity self) {
  Transform2 *t = ecs_get(sc, self, Transform2);
  Rectangle rec = {t->position.x, t->position.y, 16, 16};
  DrawRectangleRec(rec, RED);
}

void gui_self(Scene *sc, Entity self) {
  RigidBody *rb = ecs_get(sc, self, RigidBody);

  char acctxt[16];
  snprintf(acctxt, 14, "ACC: %.5f\n", Vector2Length(rb->acc));
  DrawText(acctxt, 10, 10, 16, WHITE);
  char sptxt[16];
  snprintf(sptxt, 14, "SPD: %.5f\n", Vector2Length(rb->speed));
  DrawText(sptxt, 10, 40, 16, WHITE);

  DrawText("MOVE: WASD", 700, 10, 14, WHITE);
}

int main(void) {

  InitWindow(SCREEN_W, SCREEN_H, "Default Components");
  Camera2D cam = {{SCREEN_W / 2.f, SCREEN_H / 2.f}, {0, 0}, 0, 1.f};
  Color bg = {0, 0, 0, 255};

  FalsECS falsecs = falsecs_start(bg);
  Scene *sc = falsecs_scene(&falsecs, cam);

  Entity e = ecs_entity_wdata(sc);
  ecs_add(sc, e, Transform2, TRANSFORM_ZERO);

  float mass = 40.f;
  float dam = 2.5f;
  ecs_add(sc, e, RigidBody, RIGIDBODY_STATIC(mass, dam));

  Collider col = collider_solid(3, 24);
  ecs_add_obj(sc, e, Collider, col);

  ecs_add(sc, e, Behaviour, {0});
  ecs_script(sc, e, move_self, EcsOnFixedUpdate);
  ecs_script(sc, e, draw_self, EcsOnRender);
  ecs_script(sc, e, gui_self, EcsOnGui);

  Entity e2 = ecs_entity_wdata(sc);
  Entity e3 = ecs_entity_wdata(sc);
  Entity e4 = ecs_entity_wdata(sc);

  ecs_add(sc, e2, Transform2, {{50, 80}, {1, 1}, 0});
  ecs_add(sc, e3, Transform2, {{-90, 30}, {1, 1}, 0});
  ecs_add(sc, e4, Transform2, {{-400, -80}, {1, 1}, 0});

  Collider col2 = collider_solid(5, 40);
  Collider col3 = collider_hollow(4, 20);
  Collider col4 = collider_solid(8, 32);
  ecs_add_obj(sc, e2, Collider, col2);
  ecs_add_obj(sc, e3, Collider, col3);
  ecs_add_obj(sc, e4, Collider, col4);

  ecs_add(sc, e4, RigidBody, RIGIDBODY_STATIC(mass + 10, dam));
  RigidBody *rbE4 = ecs_get(sc, e4, RigidBody);
  rbE4->acc.x = 40;

  ecs_system(sc, EcsOnRender, ecs_debug_collider_system, Transform2, Collider);

  falsecs_loop(&falsecs);
  falsecs_clean(&falsecs);
  CloseWindow();

  return 0;
}
