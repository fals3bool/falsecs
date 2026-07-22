#include <ecs/component.h>
#include <ecs/system.h>

static Color normal = {102, 191, 255, 128};
static Color hit = {230, 41, 55, 128};

void DebugColliderSystem(ECS *ecs, Entity e) {
  Collider *c = GetComponent(ecs, e, Collider);
  switch (c->type) {
  case Box: {
    BoxShape *s = (BoxShape *)c->shape;
    DrawCube(s->center, s->width, s->height, s->length, c->overlap ? hit : normal);
    DrawCubeWires(s->center, s->width, s->height, s->length, c->overlap ? MAROON : BLUE);
  } break;

  case Sphere: {
    SphereShape *s = (SphereShape *)c->shape;
    DrawSphere(s->center, s->radius, c->overlap ? hit : normal);
  } break;

  case Capsule: {
    CapsuleShape *s = (CapsuleShape *)c->shape;
    DrawCapsule(s->top, s->bottom, s->radius, 10, 10, c->overlap ? hit : normal);
  } break;

  case Convex: {
    ConvexShape *s = (ConvexShape *)c->shape;
    for (uint8_t p = 0; p < s->vertices - 1; p++) {
      for (uint8_t q = p + 1; q < s->vertices; q++) {
        DrawLine3D(s->vx[p], s->vx[q], c->overlap ? MAROON : BLUE);
      }
    }
  } break;

  default:
    break;
  }
}
