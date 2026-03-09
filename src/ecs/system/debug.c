#include <ecs/component.h>
#include <ecs/system.h>

void DebugColliderSystem(ECS *ecs, Entity e) {
  Collider *col = GetComponent(ecs, e, Collider);
  for (uint8_t i = 0; i < col->vertices; i++) {
    Vector2 p = col->vx[i];
    Vector2 q = col->vx[(i + 1) % col->vertices];
    DrawLineEx(p, q, 2, col->overlap ? RED : SKYBLUE);
  }
}
