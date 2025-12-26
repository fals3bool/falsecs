#include <ecs/component.h>
#include <ecs/system.h>

void ecs_physics_system(Registry *r, Entity e) {
  RigidBody *rb = ecs_get(r, e, RigidBody);
  Transform2 *t = ecs_get(r, e, Transform2);

  rb->speed.x += rb->acc.x * ECS_FIXED_DELTATIME;
  rb->speed.y += rb->acc.y * ECS_FIXED_DELTATIME;

  t->position.x += rb->speed.x * ECS_FIXED_DELTATIME;
  t->position.y += rb->speed.y * ECS_FIXED_DELTATIME;

  if(rb->damping > 0.f)
    rb_apply_damping(rb);
}

void ecs_gravity_system(Registry *r, Entity e) {
  RigidBody *rb = ecs_get(r, e, RigidBody);
  if (rb->is_static)
    return;

  Vector2 w = {0, 9.8f * rb->mass};
  rb_apply_force(rb, w);
}
