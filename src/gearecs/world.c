#include <gearecs/world.h>

#ifdef PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif

static float fixed_time; ///< Accumulator for fixed timestep integration
static Color background; ///< Window background color

ECS *EcsWorld(void) {
  ECS *ecs = EcsRegistry();

  Component(ecs, Transform);
  Component(ecs, LocalTransform);
  Component(ecs, Behaviour);
  Component(ecs, Parent);
  ComponentDynamic(ecs, Children, ChildrenDestructor);
  Component(ecs, Camera);
  Component(ecs, Sprite);
  ComponentDynamic(ecs, Collider, ColliderDestructor);
  Component(ecs, CollisionListener);
  Component(ecs, RigidBody);

  Camera3D camera = {0};
  camera.position = (Vector3){0.0f, 0.0f, 30.0f};
  camera.target = (Vector3){0.0f, 0.0f, 0.0f};
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  camera.fovy = 145.0f;
  camera.projection = CAMERA_ORTHOGRAPHIC;
  Entity camEntity = EcsEntity(ecs, "MainCamera");
  AddComponent(ecs, camEntity, Camera, camera);

  System(ecs, BehaviourStartSystem, EcsOnStart, Behaviour);
  System(ecs, BehaviourUpdateSystem, EcsOnUpdate, Behaviour);
  System(ecs, BehaviourLateSystem, EcsOnLateUpdate, Behaviour);
  System(ecs, BehaviourFixedSystem, EcsOnFixedUpdate, Behaviour);
  System(ecs, BehaviourRenderSystem, EcsOnRender, Behaviour);
  System(ecs, BehaviourGuiSystem, EcsOnGui, Behaviour);

  System(ecs, HierarchyTransformSystem, EcsOnUpdate, Transform, Parent);
  System(ecs, TransformColliderSystem, EcsOnUpdate, Transform, Collider);
  System(ecs, CollisionSystem, EcsOnUpdate, Transform, Collider);

  System(ecs, GravitySystem, EcsOnFixedUpdate, RigidBody);
  System(ecs, PhysicsSystem, EcsOnFixedUpdate, RigidBody, Transform);

  System(ecs, SpriteSystem, EcsOnRender, Transform, Sprite);

  AddLayer(ecs, "default");

  background = (Color){23, 28, 29, 255};
  return ecs;
}

void GameGenericLoop(void *world) {
  ECS *ecs = (ECS *)world;

  EcsRunSystems(ecs, EcsOnUpdate);
  EcsRunSystems(ecs, EcsOnLateUpdate);

  fixed_time += GetFrameTime();
  while (fixed_time >= FIXED_DELTATIME) {
    EcsRunSystems(ecs, EcsOnFixedUpdate);
    fixed_time -= FIXED_DELTATIME;
  }

  BeginDrawing();
  ClearBackground(background);

  Camera *cam = WorldMainCamera(ecs);
  BeginMode3D(*cam);
  EcsRunSystems(ecs, EcsOnRender);
  EndMode3D();

  EcsRunSystems(ecs, EcsOnGui);
  EndDrawing();
}

void EcsLoop(ECS *world) {
  if (!world)
    return;
  EcsRunSystems(world, EcsOnStart);
#ifdef PLATFORM_WEB
  emscripten_set_main_loop_arg(GameGenericLoop, world, 0, 1);
#else
  while (!WindowShouldClose()) {
    GameGenericLoop(world);
  }
#endif
}

Camera *WorldMainCamera(ECS *ecs) { return GetComponent(ecs, 0, Camera); }
