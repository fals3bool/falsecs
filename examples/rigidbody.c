#include "ecs/component.h"
#include <scene/manager.h>

#include <raymath.h>

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

  // modify speed:
  rb_apply_impulse(rb, Vector2Scale(Vector2Normalize(d), 0.3f));
  // modify acceleration:
  // rb_apply_force(rb, Vector2Scale(Vector2Normalize(d), 0.3f));
  // won't stop unless manually stoped.
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
  ecs_add(sc, e, Transform2, TRANSFORM_DEFAULT);

  // add rigidbody
  // ecs_add(sc, e, RigidBody, {20, 0.9f}); // mass = 20kg, damping = 0.9.
  // ecs_add(sc, e, RigidBody, {20, 0.9f, 0}) // with gravity.
  // ecs_add(sc, e, RigidBody, {20, 0.9f, 1}) // without gravity.
  //
  // there are some macros available.
  float mass = 4.f;
  float dam = 1.1f;
  // ecs_add(sc, e, RigidBody, RIGIDBODY_DYNAMIC(mass, dam)); // with gravity
  ecs_add(sc, e, RigidBody, RIGIDBODY_STATIC(mass, dam)); // without gravity

  ecs_add(sc, e, Behaviour, BEHAVIOUR_DEFAULT);
  ecs_script(sc, e, move_self, EcsOnUpdate);
  ecs_script(sc, e, draw_self, EcsOnRender);

  falsecs_loop(&falsecs);
  falsecs_clean(&falsecs);
  CloseWindow();

  return 0;
}
