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

  d = Vector2Scale(Vector2Normalize(d), 900.f);

  if (IsKeyDown(KEY_SPACE))
    d = Vector2Scale(d, 3);

  // modify speed:
  rb_apply_impulse(rb, d);

  // modify acceleration:
  // rb_apply_force(rb, Vector2Scale(Vector2Normalize(d), 900.f));
  // won't stop unless manually stoped.
  // rb_apply_force(rb, (Vector2){-rb->acc.x, -rb->acc.y});

  if (t->position.y < -SCREEN_H / 2.f)
    t->position.y = SCREEN_H / 2.f;
  if (t->position.y > SCREEN_H / 2.f)
    t->position.y = -SCREEN_H / 2.f;
  if (t->position.x < -SCREEN_W / 2.f)
    t->position.x = SCREEN_W / 2.f;
  if (t->position.x > SCREEN_W / 2.f)
    t->position.x = -SCREEN_W / 2.f;
}

void draw_self(Scene *sc, Entity self) {
  Transform2 *t = ecs_get(sc, self, Transform2);
  Rectangle rec = {t->position.x - 8, t->position.y - 8, 16, 16};
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
  DrawText("DASH: SPACE", 700, 40, 14, WHITE);
}

int main(void) {

  InitWindow(SCREEN_W, SCREEN_H, "Default Components");
  Camera2D cam = {{SCREEN_W / 2.f, SCREEN_H / 2.f}, {0, 0}, 0, 1.f};
  Color bg = {0, 0, 0, 255};

  FalsECS falsecs = falsecs_start(bg);
  Scene *sc = falsecs_scene(&falsecs, cam);

  Entity e = ecs_entity_wdata(sc);
  ecs_add(sc, e, Transform2, TRANSFORM_ZERO);

  // add rigidbody
  ecs_add(sc, e, RigidBody, {20}); // mass = 20kg.
  ecs_add(sc, e, RigidBody, {20, 0.9f}); // mass = 20kg, damping = 0.9.
  ecs_add(sc, e, RigidBody, {20, 0.9f, 0}) // with gravity.
  ecs_add(sc, e, RigidBody, {20, 0.9f, 1}) // without gravity.
  
  // there are some macros available.
  float mass = 40.f;
  float dam = 2.7f;
  ecs_add(sc, e, RigidBody, RIGIDBODY_DYNAMIC(mass, dam)); // with gravity
  ecs_add(sc, e, RigidBody, RIGIDBODY_STATIC(mass, dam)); // without gravity

  ecs_add(sc, e, Behaviour, {0});
  ecs_script(sc, e, move_self, EcsOnFixedUpdate);
  ecs_script(sc, e, draw_self, EcsOnRender);
  ecs_script(sc, e, gui_self, EcsOnGui);

  falsecs_loop(&falsecs);
  falsecs_clean(&falsecs);
  CloseWindow();

  return 0;
}
