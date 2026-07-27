#include "ecs/component.h"
#include <gearecs.h>

typedef struct {
  float speed;
  float jump;
  bool cooldown;
} PlayerData;

void ScriptMove(ECS *ecs, Entity self) {
  RigidBody *rb = GetComponent(ecs, self, RigidBody);
  PlayerData *pd = GetComponent(ecs, self, PlayerData);
  Transform *t = GetComponent(ecs, self, Transform);

  if (IsKeyDown(KEY_W))
    ApplyImpulse(rb, (Vector3){0, 0, -pd->speed});
  if (IsKeyDown(KEY_S))
    ApplyImpulse(rb, (Vector3){0, 0, pd->speed});
  if (IsKeyDown(KEY_A))
    ApplyImpulse(rb, (Vector3){-pd->speed, 0, 0});
  if (IsKeyDown(KEY_D))
    ApplyImpulse(rb, (Vector3){pd->speed, 0, 0});

  if (IsKeyDown(KEY_SPACE) && !pd->cooldown) {
    ApplyImpulse(rb, (Vector3){0, pd->jump, 0});
    pd->cooldown = true;
  }

  if (t->translation.y < -150)
    t->translation.y = 150;
}

void OnGround(ECS *ecs, CollisionEvent *event) {
  RigidBody *rb = GetComponent(ecs, event->self, RigidBody);
  PlayerData *pd = GetComponent(ecs, event->self, PlayerData);
  rb->acc = (Vector3){0, 0, 0};
  pd->cooldown = false;
}

int main(void) {
  InitWindow(800, 450, "Getting Started with GearECS!");

  ECS *world = EcsWorld();
  Component(world, PlayerData);

  Camera *cam = WorldMainCamera(world);
  cam->position = (Vector3){10.0f, 5.0f, 10.0f};
  cam->target = (Vector3){0.0f, 0.0f, 0.0f};
  cam->up = (Vector3){0.0f, 1.0f, 0.0f};
  cam->fovy = 60.0f;
  cam->projection = CAMERA_ORTHOGRAPHIC;

  Entity box = EcsEntity(world, "box");
  AddComponent(world, box, PlayerData, {1, 20, true});
  AddComponent(world, box, Transform, TransformOrigin);
  AddComponent(world, box, Collider, ColliderBox(1, 1, 1, true));
  AddComponent(world, box, CollisionListener, {OnGround});
  AddComponent(world, box, RigidBody, RigidBodyDynamic(0.5, 3));
  AddScript(world, box, ScriptMove, EcsOnFixedUpdate);

  Entity floor = EcsEntity(world, "floor");
  AddComponent(world, floor, Transform, TransformPosition(0, -10, 0));
  AddComponent(world, floor, Collider, ColliderBox(20, 1, 20, true));
  AddComponent(world, floor, RigidBody, RigidBodyStatic);

  System(world, DebugColliderSystem, EcsOnRender, Collider, Transform);
  EcsLoop(world);

  EcsFree(world);
  CloseWindow();
  return 0;
}
