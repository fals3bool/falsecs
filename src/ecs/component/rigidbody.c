#include <ecs/component.h>

void ApplyForce(RigidBody *rb, Vector3 force) {
  rb->acc = Vector3Add(rb->acc, Vector3Scale(force, rb->invmass));
}

void ApplyImpulse(RigidBody *rb, Vector3 impulse) {
  rb->speed = Vector3Add(rb->speed, Vector3Scale(impulse, rb->invmass));
}

void ApplyDamping(RigidBody *rb) {
  float fac = expf(-rb->damping * FIXED_DELTATIME);
  rb->speed = Vector3Scale(rb->speed, fac);
}
