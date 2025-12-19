#include <ecs/system.h>
#include <ecs/component.h>

void ecs_sprite_system(Registry *r, Entity e) {
  Transform2 *t = ecs_get(r,e , Transform2);
  Sprite *sp = ecs_get(r, e, Sprite);

  float tw = sp->src.width;
  float th = sp->src.height;
  Rectangle dest = {t->position.x, t->position.y, tw, th};
  Vector2 origin = {tw/2, th/2};
  DrawTexturePro(sp->tex, sp->src, dest, origin, t->rotation * RAD2DEG, sp->tint);
}
