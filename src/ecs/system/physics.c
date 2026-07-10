#include <ecs/component.h>
#include <ecs/system.h>

void PhysicsSystem(ECS *ecs, Entity e) {
  RigidBody *rb = GetComponent(ecs, e, RigidBody);
  Transform *t = GetComponent(ecs, e, Transform);

  rb->speed.x += rb->acc.x * FIXED_DELTATIME;
  rb->speed.y += rb->acc.y * FIXED_DELTATIME;
  rb->speed.z += rb->acc.z * FIXED_DELTATIME;

  t->translation.x += rb->speed.x * FIXED_DELTATIME;
  t->translation.y += rb->speed.y * FIXED_DELTATIME;
  t->translation.z += rb->speed.z * FIXED_DELTATIME;

  if (rb->damping > 0.f)
    ApplyDamping(rb);
}

void GravitySystem(ECS *ecs, Entity e) {
  RigidBody *rb = GetComponent(ecs, e, RigidBody);
  if (!(rb->type == BodyDynamic && rb->gravity))
    return;

  Vector3 w = {0, -9.8f, 0};
  ApplyForce(rb, w);
}
