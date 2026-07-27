#include "raylib.h"
#include <gearecs.h>

void MoveScript(ECS *ecs, Entity self) {
  // Get transform component
  Transform *t = GetComponent(ecs, self, Transform);

  // Keyboard
  Vector3 d = {0};
  if (IsKeyDown(KEY_UP))
    d.y += 1;
  if (IsKeyDown(KEY_DOWN))
    d.y -= 1;
  if (IsKeyDown(KEY_LEFT))
    d.x -= 1;
  if (IsKeyDown(KEY_RIGHT))
    d.x += 1;

  // Move
  t->translation = Vector3Add(t->translation, Vector3Scale(d, 100 * GetFrameTime()));
}

int main(void) {
  // Raylib window and camera
  InitWindow(800, 450, "GearECS Example");
  ToggleFullscreen();

  // Registry
  ECS *world = EcsWorld();

  // Player entity
  Entity player = EcsEntity(world, "Player");
  AddComponent(world, player, Transform, TransformOrigin);
  AddComponent(world, player, Collider, ColliderBox(10, 10, 10, false));
  AddScript(world, player, MoveScript, EcsOnUpdate);

  // Optional (debug) system
  System(world, DebugColliderSystem, EcsOnRender, Collider);

  // Main game loop
  EcsLoop(world);

  // Cleanup
  EcsFree(world);
  CloseWindow();
  return 0;
}
