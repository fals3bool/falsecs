#include <ecs/component.h>
#include <ecs/system.h>

void HierarchyTransformSystem(ECS *ecs, Entity e) {
  Hierarchy *h = GetComponent(ecs, e, Hierarchy);
  if (h->parent == InvalidID)
    return;
  Transform *t = GetComponent(ecs, e, Transform);
  LocalTransform *tl = GetComponent(ecs, e, LocalTransform);
  Transform *tp = GetComponent(ecs, h->parent, Transform);
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

  switch (c->type) {
  case Box: {
    BoxShape *s = (BoxShape *)c->shape;
    s->center = t->translation;
  } break;

  case Sphere: {
    SphereShape *s = (SphereShape *)c->shape;
    s->center = t->translation;
  } break;

  case Capsule: {
    CapsuleShape *s = (CapsuleShape *)c->shape;
    float h = s->height * 0.5f;
    Vector3 top = QuaternionRotateVector(t->rotation, (Vector3){0, h, 0});
    s->top.x = top.x + t->translation.x;
    s->top.y = top.y + t->translation.y;
    s->top.z = top.z + t->translation.z;
    Vector3 bottom = QuaternionRotateVector(t->rotation, (Vector3){0, -h, 0});
    s->bottom.x = bottom.x + t->translation.x;
    s->bottom.y = bottom.y + t->translation.y;
    s->bottom.z = bottom.z + t->translation.z;
  } break;

  case Convex: {
    ConvexShape *s = (ConvexShape *)c->shape;
    for (uint8_t i = 0; i < s->vertices; i++) {
      Vector3 v = QuaternionRotateVector(t->rotation, s->md[i]);
      s->vx[i].x = v.x + t->translation.x;
      s->vx[i].y = v.y + t->translation.y;
      s->vx[i].z = v.z + t->translation.z;
    }
  } break;

  default:
    break;
  }

  c->overlap = false;
}
