#include <scene/manager.h>

#include <stdio.h>
#include <stdlib.h>

Entity falsecs_entity(Scene *sc) {
  Entity e = ecs_entity(sc);
  ecs_add_def(sc, e, EntityData, ENTITYDATA_DEFAULT);
  return e;
}

FalsECS *falsecs_start(int screen_w, int screen_h, char *title, Color bg) {
  InitWindow(screen_w, screen_h, title);
  FalsECS *falsecs = malloc(sizeof(FalsECS));
  falsecs->sc_w = screen_w;
  falsecs->sc_h = screen_h;
  falsecs->scene = NULL;
  falsecs->bg = bg;
  return falsecs;
}

void falsecs_loop(FalsECS *falsecs) {

  printf("entities: %d\n", ecs_entity_count(falsecs->scene));
  ecs_run(falsecs->scene, EcsOnStart);

  float time = 0.f;
  while (!WindowShouldClose()) {
    ecs_run(falsecs->scene, EcsOnUpdate);
    ecs_run(falsecs->scene, EcsOnLateUpdate);

    time += GetFrameTime();
    while (time >= FIXED_DELTATIME) {
      ecs_run(falsecs->scene, EcsOnFixedUpdate);
      time -= FIXED_DELTATIME;
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
}

Scene *falsecs_scene(FalsECS *falsecs) {
  Scene *r = ecs_registry();

  ecs_component(r, Camera2D);
  ecs_component(r, EntityData);
  ecs_component(r, Transform2);
  ecs_component(r, Sprite);
  ecs_component(r, Behaviour);

  Entity cam = ecs_entity(r);
  ecs_add(r, cam, Camera2D,
          {{falsecs->sc_w / 2.f, falsecs->sc_h / 2.f}, {0, 0}, 0, 1});

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

  falsecs->scene = r;
  return r;
}

void falsecs_free_scene(FalsECS *falsecs) {
  if (falsecs->scene == NULL)
    return;
  ecs_registry_free(falsecs->scene);
  falsecs->scene = NULL;
}

void falsecs_free(FalsECS *falsecs) {
  CloseWindow();
  falsecs_free_scene(falsecs);
  free(falsecs);
}
