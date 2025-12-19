#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <ecs/component.h>
#include <ecs/system.h>

#define FIXED_DELTATIME 1.f / 60.f

typedef Registry Scene;

typedef struct {
  int sc_w;
  int sc_h;
  Color bg;
  Scene *scene;
} FalsECS;

Entity falsecs_entity(Scene *sc);

FalsECS *falsecs_start(int screen_w, int screen_h, char *title, Color bg);
void falsecs_loop(FalsECS *falsecs);

Scene *falsecs_scene(FalsECS *falsecs);
void falsecs_free_scene(FalsECS *falsecs);
void falsecs_free(FalsECS *falsecs);

#endif
