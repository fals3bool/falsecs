#include "ecs/system.h"
#include <scene/manager.h>

#include <stdlib.h>

#ifdef PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif

Entity ecs_entity_wdata(Scene *sc) {
  Entity e = ecs_entity(sc);
  ecs_add(sc, e, EntityData, ENTITYDATA_DEFAULT);
  return e;
}

FalsECS falsecs_start(Color bg) {
  FalsECS falsecs = {bg, 0, NULL};
  return falsecs;
}

void generic_loop(void *manager) {
  FalsECS *falsecs = (FalsECS *)manager;

  ecs_run(falsecs->scene, EcsOnUpdate);
  ecs_run(falsecs->scene, EcsOnLateUpdate);

  falsecs->fixed_time += GetFrameTime();
  while (falsecs->fixed_time >= ECS_FIXED_DELTATIME) {
    ecs_run(falsecs->scene, EcsOnFixedUpdate);
    falsecs->fixed_time -= ECS_FIXED_DELTATIME;
  }

  BeginDrawing();
  ClearBackground(falsecs->bg);

  Camera2D *cam = ecs_get(falsecs->scene, 0, Camera2D);
  BeginMode2D(*cam);
  ecs_run(falsecs->scene, EcsOnRender);
  EndMode2D();

  ecs_run(falsecs->scene, EcsOnGui);
  EndDrawing();
}

void falsecs_loop(FalsECS *falsecs) {
  ecs_run(falsecs->scene, EcsOnStart);
#ifdef PLATFORM_WEB
  emscripten_set_main_loop_arg(generic_loop, falsecs, 0, 1);
#else
  while (!WindowShouldClose()) {
    generic_loop(falsecs);
  }
#endif
}

Scene *falsecs_scene(FalsECS *falsecs, Camera2D camera) {
  Scene *r = ecs_registry();

  ecs_component(r, Camera2D);
  ecs_component(r, EntityData);
  ecs_component(r, Transform2);
  ecs_component(r, Sprite);
  ecs_component(r, Behaviour);
  ecs_component(r, Collider);
  ecs_component(r, RigidBody);

  Entity cam = ecs_entity(r);
  ecs_add_obj(r, cam, Camera2D, camera);

  ecs_system(r, EcsOnStart, ecs_behaviour_system_start, Behaviour, EntityData);
  ecs_system(r, EcsOnUpdate, ecs_behaviour_system_update, Behaviour,
             EntityData);
  ecs_system(r, EcsOnLateUpdate, ecs_behaviour_system_late, Behaviour,
             EntityData);
  ecs_system(r, EcsOnFixedUpdate, ecs_behaviour_system_fixed, Behaviour,
             EntityData);
  ecs_system(r, EcsOnRender, ecs_behaviour_system_render, Behaviour,
             EntityData);
  ecs_system(r, EcsOnGui, ecs_behaviour_system_gui, Behaviour, EntityData);

  ecs_system(r, EcsOnUpdate, ecs_transform_collider_system, Transform2,
             Collider);
  ecs_system(r, EcsOnUpdate, ecs_collision_system, Transform2, Collider);

  ecs_system(r, EcsOnFixedUpdate, ecs_gravity_system, RigidBody);
  ecs_system(r, EcsOnFixedUpdate, ecs_physics_system, RigidBody, Transform2);

  ecs_system(r, EcsOnRender, ecs_sprite_system, Transform2, Sprite);

  falsecs->scene = r;
  return r;
}

void falsecs_clean(FalsECS *falsecs) {
  if (falsecs->scene == NULL)
    return;
  ecs_registry_free(falsecs->scene);
  falsecs->scene = NULL;
}
