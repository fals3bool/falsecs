#include <ecs/component.h>
#include <ecs/system.h>

// TODO: render quads
void SpriteSystem(ECS *ecs, Entity e) {
  // Transform2 *t = GetComponent(ecs, e, Transform2);
  // Sprite *sp = GetComponent(ecs, e, Sprite);
  //
  // float tw = sp->src.width * t->scale.x;
  // float th = sp->src.height * t->scale.y;
  // Rectangle dest = {t->position.x, t->position.y, tw, th};
  // Vector2 origin = {tw / 2, th / 2};
  // DrawTexturePro(sp->tex, sp->src, dest, origin, t->rotation * RAD2DEG,
  //                sp->tint);
}
