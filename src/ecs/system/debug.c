#include <ecs/component.h>
#include <ecs/system.h>

void DebugColliderSystem(ECS *ecs, Entity e) {
  Collider *c = GetComponent(ecs, e, Collider);
  for (uint8_t i = 0; i < c->edges; i++) {
    Vector3 p = c->vx[c->edge[i * 2]];
    Vector3 q = c->vx[c->edge[i * 2 + 1]];
    DrawLine3D(p, q, c->overlap ? RED : SKYBLUE);
  }
}
