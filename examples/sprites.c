#include <scene/manager.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_W 800
#define SCREEN_H 450

Vector2 random_pos() {
  Vector2 pos;
  pos.x = rand() % 400 - 200;
  pos.y = rand() % 400 - 200;
  return pos;
}

Vector2 random_scale() {
  Vector2 scale;
  scale.x = rand() % 4 + 1;
  scale.y = rand() % 4 + 1;
  return scale;
}

void transform_system(Registry *r, Entity e) {
  Transform2 *t = ecs_get(r, e, Transform2);
  t->rotation += GetFrameTime();
}

int main(void) {
  InitWindow(SCREEN_W, SCREEN_H, "Sprites");
  Camera2D cam = {0};
  cam.offset = (Vector2){SCREEN_W / 2.f, SCREEN_H / 2.f};
  cam.zoom = 1.f;

  FalsECS man = falsecs_start(BLACK);
  Scene *s = falsecs_scene(&man, cam);

  Image img = GenImageColor(32, 32, WHITE);
  Texture2D tex = LoadTextureFromImage(img);
  UnloadImage(img);

  Entity e0 = ecs_entity_wdata(s);
  Entity e1 = ecs_entity_wdata(s);
  Entity e2 = ecs_entity_wdata(s);
  Entity e3 = ecs_entity_wdata(s);
  Entity e4 = ecs_entity_wdata(s);
  Entity e5 = ecs_entity_wdata(s);
  Entity e6 = ecs_entity_wdata(s);

  srand(time(NULL));

  ecs_add(s, e0, Sprite, {tex, {0, 0, 32, 32}, BLUE});
  ecs_add(s, e0, Transform2, {random_pos(), random_scale(), PI / 4});

  ecs_add(s, e1, Sprite, {tex, {0, 0, 32, 32}, RED});
  ecs_add(s, e1, Transform2, {random_pos(), random_scale(), PI / 3});

  ecs_add(s, e2, Sprite, {tex, {0, 0, 32, 32}, ORANGE});
  ecs_add(s, e2, Transform2, {random_pos(), random_scale(), PI / 6});

  ecs_add(s, e3, Sprite, {tex, {0, 0, 32, 32}, PURPLE});
  ecs_add(s, e3, Transform2, {random_pos(), random_scale(), PI / 2});

  ecs_add(s, e4, Sprite, {tex, {0, 0, 32, 32}, GREEN});
  ecs_add(s, e4, Transform2, {random_pos(), random_scale(), PI / 8});

  ecs_add(s, e5, Sprite, {tex, {0, 0, 32, 32}, PINK});
  ecs_add(s, e5, Transform2, {random_pos(), random_scale(), PI / 7});

  ecs_add(s, e6, Sprite, {tex, {0, 0, 32, 32}, YELLOW});
  ecs_add(s, e6, Transform2, {random_pos(), random_scale(), PI / 9});

  ecs_system(s, EcsOnUpdate, transform_system, Transform2);

  falsecs_loop(&man);
  falsecs_clean(&man);

  UnloadTexture(tex);

  CloseWindow();
}
