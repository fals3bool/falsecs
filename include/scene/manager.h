#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <ecs/component.h>
#include <ecs/system.h>

typedef Registry Scene;

typedef struct {
  Color bg;
  float fixed_time;
  Scene *scene;
} FalsECS;

Entity ecs_entity_wdata(Scene *sc);

FalsECS falsecs_start(Color bg);
void falsecs_loop(FalsECS *falsecs);

Scene *falsecs_scene(FalsECS *falsecs, Camera2D camera);
void falsecs_clean(FalsECS *falsecs);

#endif
