#include <ecs/component.h>

#include <stdlib.h>
#include <string.h>

Collider ColliderBox(float width, float height, float length, bool solid) {
  Collider c = {0};
  BoxShape shape = {{0, 0, 0}, width, height, length};

  c.type = Box;
  c.shape = malloc(sizeof(BoxShape));
  memcpy(c.shape, &shape, sizeof(BoxShape));

  c.solid = solid;
  return c;
}

Collider ColliderCapsule(float radius, float height, bool solid) {
  Collider c = {0};
  float h = height * 0.5f;
  CapsuleShape shape = {{0, h, 0}, {0, -h, 0}, radius, height};

  c.type = Capsule;
  c.shape = malloc(sizeof(CapsuleShape));
  memcpy(c.shape, &shape, sizeof(CapsuleShape));

  c.solid = solid;
  return c;
}

Collider ColliderSphere(float radius, bool solid) {
  Collider c = {0};
  SphereShape shape = {{0, 0, 0}, radius};

  c.type = Sphere;
  c.shape = malloc(sizeof(SphereShape));
  memcpy(c.shape, &shape, sizeof(SphereShape));

  c.solid = solid;
  return c;
}

Collider ColliderConvex(Vector3 *model, uint8_t vertices, bool solid) {
  Collider c = {0};
  ConvexShape shape = {0};

  shape.vertices = vertices;
  shape.md = (Vector3 *)malloc(sizeof(Vector3) * vertices);
  shape.vx = (Vector3 *)malloc(sizeof(Vector3) * vertices);
  memcpy(shape.md, model, sizeof(Vector3) * vertices);
  memcpy(shape.vx, model, sizeof(Vector3) * vertices);

  c.type = Convex;
  c.shape = malloc(sizeof(ConvexShape));
  memcpy(c.shape, &shape, sizeof(ConvexShape));

  c.solid = solid;
  return c;
}

void ColliderDestructor(void *_self) {
  Collider *self = (Collider *)_self;
  if (self->shape == NULL)
    return;
  if (self->type == Convex) {
    ConvexShape *s = (ConvexShape *)self->shape;
    if (s->vertices > 0) {
      free(s->vx);
      free(s->md);
    }
  }
  free(self->shape);
}
