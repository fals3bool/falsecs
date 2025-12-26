#include <ecs/component.h>

#include <math.h>

void rb_apply_force(RigidBody *rb, Vector2 force) {
  rb->acc.x += force.x / rb->mass;
  rb->acc.y += force.y / rb->mass;
}

void rb_apply_impulse(RigidBody *rb, Vector2 impulse) {
  rb->speed.x += impulse.x / rb->mass;
  rb->speed.y += impulse.y / rb->mass;
}

void rb_apply_damping(RigidBody *rb) {
  float fac = expf(-rb->damping * ECS_FIXED_DELTATIME);
  rb->speed.x *= fac;
  rb->speed.y *= fac;
}
