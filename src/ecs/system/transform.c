#include <ecs/component.h>
#include <ecs/system.h>

void HierarchyTransformSystem(ECS *ecs, Entity e) {
  Parent *p = GetComponent(ecs, e, Parent);
  Transform *t = GetComponent(ecs, e, Transform);
  LocalTransform *tl = GetComponent(ecs, e, LocalTransform);
  Transform *tp = GetComponent(ecs, p->entity, Transform);
  if (!tp)
    return;
  t->translation = Vector3Add(tp->translation, tl->translation);
  t->scale = (Vector3){tp->scale.x * tl->scale.x, tp->scale.y * tl->scale.y,
                       tp->scale.z * tl->scale.z};
  t->rotation = QuaternionAdd(t->rotation, tl->rotation);
}

static Vector3 QuaternionRotateVector(Quaternion q, Vector3 v) {
  Vector3 qv = {q.x, q.y, q.z};

  Vector3 t = Vector3CrossProduct(qv, v);
  t.x *= 2.0f;
  t.y *= 2.0f;
  t.z *= 2.0f;

  Vector3 res;

  res.x = v.x + q.w * t.x + (qv.y * t.z - qv.z * t.y);
  res.y = v.y + q.w * t.y + (qv.z * t.x - qv.x * t.z);
  res.z = v.z + q.w * t.z + (qv.x * t.y - qv.y * t.x);

  return res;
}

void TransformColliderSystem(ECS *ecs, Entity e) {
  Transform *t = GetComponent(ecs, e, Transform);
  Collider *c = GetComponent(ecs, e, Collider);

  for (uint8_t i = 0; i < c->vertices; i++) {
    Vector3 v = QuaternionRotateVector(t->rotation, c->md[i]);
    c->vx[i].x = v.x + t->translation.x;
    c->vx[i].y = v.y + t->translation.y;
    c->vx[i].z = v.z + t->translation.z;
  }

  c->overlap = false;
}
