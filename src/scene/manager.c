#include "ecs/system.h"
#include <scene/manager.h>

#include <stdlib.h>

#ifdef PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif

Entity EcsEntityData(ECS *ecs) {
  Entity e = EcsEntity(ecs);
  EcsAdd(ecs, e, EntityData, ENTITYDATA_ACTIVE);
  return e;
}

GameScene SceneStart(uint16_t max_entities, Camera2D camera) {
  ECS *ecs = EcsRegistry(max_entities);

  EcsComponent(ecs, EntityData);
  EcsComponent(ecs, Transform2);
  EcsComponent(ecs, Camera2D);
  EcsComponent(ecs, Sprite);
  EcsComponent(ecs, Behaviour);
  EcsComponent(ecs, Collider);
  EcsComponent(ecs, RigidBody);

  Entity camEntity = EcsEntity(ecs);
  EcsAddExt(ecs, camEntity, Camera2D, camera);

  EcsSystem(ecs, BehaviourStartSystem, EcsOnStart, Behaviour);
  EcsSystem(ecs, BehaviourUpdateSystem, EcsOnUpdate, Behaviour);
  EcsSystem(ecs, BehaviourLateSystem, EcsOnLateUpdate, Behaviour);
  EcsSystem(ecs, BehaviourFixedSystem, EcsOnFixedUpdate, Behaviour);
  EcsSystem(ecs, BehaviourRenderSystem, EcsOnRender, Behaviour);
  EcsSystem(ecs, BehaviourGuiSystem, EcsOnGui, Behaviour);

  EcsSystem(ecs, TransformColliderSystem, EcsOnUpdate, Transform2, Collider);
  EcsSystem(ecs, CollisionSystem, EcsOnUpdate, Transform2, Collider);

  EcsSystem(ecs, GravitySystem, EcsOnFixedUpdate, RigidBody);
  EcsSystem(ecs, PhysicsSystem, EcsOnFixedUpdate, RigidBody, Transform2);

  EcsSystem(ecs, SpriteSystem, EcsOnRender, Transform2, Sprite);

  return (GameScene){ecs, (Color){23, 28, 29, 255}, 0};
}

void GameGenericLoop(void *scene) {
  GameScene *gs = (GameScene *)scene;

  EcsRun(gs->ecs, EcsOnUpdate);
  EcsRun(gs->ecs, EcsOnLateUpdate);

  gs->fixed_time += GetFrameTime();
  while (gs->fixed_time >= ECS_FIXED_DELTATIME) {
    EcsRun(gs->ecs, EcsOnFixedUpdate);
    gs->fixed_time -= ECS_FIXED_DELTATIME;
  }

  BeginDrawing();
  ClearBackground(gs->background);

  Camera2D *cam = EcsGet(gs->ecs, 0, Camera2D);
  BeginMode2D(*cam);
  EcsRun(gs->ecs, EcsOnRender);
  EndMode2D();

  EcsRun(gs->ecs, EcsOnGui);
  EndDrawing();
}

void SceneLoop(GameScene *scene) {
  EcsRun(scene->ecs, EcsOnStart);
#ifdef PLATFORM_WEB
  emscripten_set_main_loop_arg(GameGenericLoop, scene, 0, 1);
#else
  while (!WindowShouldClose()) {
    GameGenericLoop(scene);
  }
#endif
}

void SceneClean(GameScene *scene) {
  if (scene->ecs == NULL)
    return;
  EcsFree(scene->ecs);
  scene->ecs = NULL;
}
