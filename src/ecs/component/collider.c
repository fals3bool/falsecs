#include <ecs/component.h>

#include <stdlib.h>
#include <string.h>

void InitVertices(Collider *c, Vector3 *v, uint8_t len) {
  c->vertices = len;
  c->vx = (Vector3 *)malloc(sizeof(Vector3) * len);
  c->md = (Vector3 *)malloc(sizeof(Vector3) * len);
  memcpy(c->md, v, sizeof(Vector3) * len);
  memcpy(c->vx, v, sizeof(Vector3) * len);
}

void InitEdges(Collider *c, uint8_t *e, uint8_t len) {
  c->edges = len / 2;
  c->edge = (uint8_t *)malloc(len);
  memcpy(c->edge, e, len);
}

Collider ColliderCube(float size, bool solid) {
  Collider c = {0};

  float s = size / 2;
  Vector3 cube[8] = {{-s, -s, -s}, {-s, -s, s}, {s, -s, -s}, {s, -s, s},
                     {-s, s, -s},  {-s, s, s},  {s, s, -s},  {s, s, s}};
  uint8_t edges[24] = {// bottom face
                       0, 1, 0, 2, 3, 1, 3, 2,
                       // top face
                       4, 5, 4, 6, 7, 5, 7, 6,
                       // vertical edges
                       0, 4, 1, 5, 2, 6, 3, 7};

  InitVertices(&c, cube, 8);
  InitEdges(&c, edges, 24);
  c.solid = solid;
  return c;
}

void ColliderDestructor(void *_self) {
  Collider *self = (Collider *)_self;
  free(self->md);
  free(self->vx);
  free(self->edge);
}
