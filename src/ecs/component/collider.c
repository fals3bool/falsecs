#include <ecs/component.h>

#include <math.h>
#include <stdlib.h>

Collider ecs_collider_ex(int vertices, float radius, float rot, Vector2 origin,
                         uint8_t solid) {
  Collider col = {0};
  col.vx = (Vector2 *)malloc(sizeof(Vector2) * vertices);
  col.md = (Vector2 *)malloc(sizeof(Vector2) * vertices);
  col.vertices = vertices;
  col.origin = origin;
  col.rot = rot;
  col.solid = solid;
  col.overlap = 0;

  float angle = PI * 2 / vertices;
  for (int i = 0; i < vertices; i++) {
    col.vx[i] = (Vector2){radius * cosf(angle * i), radius * sinf(angle * i)};
    col.md[i] = col.vx[i];
  }

  return col;
}
