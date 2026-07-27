#include <gearecs.h>
#include <stdio.h>

typedef struct {
  float speed;
} RotationSpeed;

#define RA {0.8f}
#define RB {0.1f}
#define RC {0.4f}
#define RD {0.2f}
#define RE {0.3f}

void GridScript(ECS *ecs, Entity self) {
  (void)ecs;
  (void)self;
  DrawGrid(5, 6.0f);
}

void RotateScript(ECS *ecs, Entity self) {
  Transform *t = GetComponent(ecs, self, Transform);
  RotationSpeed *rs = GetComponent(ecs, self, RotationSpeed);

  float dt = GetFrameTime() * rs->speed;

  Quaternion dq = QuaternionFromAxisAngle((Vector3){0.0f, 1.0f, 0.0f}, dt);
  t->rotation = QuaternionMultiply(dq, t->rotation);
}

void MoveScript(ECS *ecs, Entity self) {
  RotateScript(ecs, self);
  Transform *t = GetComponent(ecs, self, Transform);
  Vector3 d = {0};
  if (IsKeyDown(KEY_UP))
    d.z -= 1;
  if (IsKeyDown(KEY_DOWN))
    d.z += 1;
  if (IsKeyDown(KEY_LEFT))
    d.x -= 1;
  if (IsKeyDown(KEY_RIGHT))
    d.x += 1;
  t->translation = Vector3Add(t->translation, Vector3Scale(d, 10 * GetFrameTime()));
}

void OnHit(ECS *ecs, CollisionEvent *event) {
  (void)ecs;
  printf("[EVENT] Collision %d -> %d: {dist: %.2f, normal: [%.2f, %.2f, %.2f]}\n",
         event->self, event->other, event->collision.distance, event->collision.normal.x,
         event->collision.normal.y, event->collision.normal.z);
}

int main(void) {

  InitWindow(800, 450, "Colliders 3D");

  ECS *ecs = EcsWorld();
  Component(ecs, RotationSpeed);

  Camera *cam = WorldMainCamera(ecs);
  cam->position = (Vector3){15.0f, 15.0f, 15.0f};
  cam->target = (Vector3){0.0f, 0.0f, 0.0f};
  cam->up = (Vector3){0.0f, 1.0f, 0.0f};
  cam->fovy = 15.0f;
  cam->projection = CAMERA_ORTHOGRAPHIC;

  AddLayer(ecs, "player");
  AddLayer(ecs, "box1");
  AddLayer(ecs, "box2");
  AddLayer(ecs, "box3");
  AddLayer(ecs, "box4");
  LayerDisable(ecs, "box1", "box2");
  LayerDisable(ecs, "box3", "player");
  LayerDisableAll(ecs, "box4");

  Entity A = EcsEntity(ecs, "A");
  float s = 1.f;
  Vector3 cube[8] = {{-s, -s, -s}, {-s, -s, s}, {s, -s, -s}, {s, -s, s},
                     {-s, s, -s},  {-s, s, s},  {s, s, -s},  {s, s, s}};
  AddComponent(ecs, A, Collider, ColliderConvex(cube, 8, true));
  AddComponent(ecs, A, Transform, TransformOrigin);
  AddComponent(ecs, A, RotationSpeed, RA);
  AddComponent(ecs, A, CollisionListener, {OnHit});
  RigidBody rbA = RigidBodyDynamic(300, 1.5f);
  rbA.gravity = false;
  AddComponent(ecs, A, RigidBody, rbA);
  AddScript(ecs, A, MoveScript, EcsOnUpdate);
  AddScript(ecs, A, GridScript, EcsOnRender);
  EntitySetLayer(ecs, A, "player");

  Entity B = EcsEntity(ecs, "B");
  AddComponent(ecs, B, Collider, ColliderBox(2, 2, 2, true));
  AddComponent(ecs, B, Transform, TransformPosition(5, 0, 5));
  AddComponent(ecs, B, RotationSpeed, RB);
  RigidBody rbB = RigidBodyDynamic(200, 1.5f);
  rbB.gravity = false;
  AddComponent(ecs, B, RigidBody, rbB);
  AddScript(ecs, B, RotateScript, EcsOnUpdate);
  EntitySetLayer(ecs, B, "box1");

  Entity C = EcsEntity(ecs, "C");
  AddComponent(ecs, C, Collider, ColliderBox(2, 2, 2, true));
  AddComponent(ecs, C, Transform, TransformPosition(-5, 0, 5));
  AddComponent(ecs, C, RotationSpeed, RC);
  AddComponent(ecs, C, RigidBody, RigidBodyStatic);
  AddScript(ecs, C, RotateScript, EcsOnUpdate);
  EntitySetLayer(ecs, C, "box2");

  Entity D = EcsEntity(ecs, "D");
  AddComponent(ecs, D, Collider, ColliderCapsule(0.5f, 1, true));
  AddComponent(ecs, D, Transform, TransformPosition(-5, 0, -5));
  AddComponent(ecs, D, RotationSpeed, RD);
  AddComponent(ecs, D, RigidBody, RigidBodyStatic);
  AddScript(ecs, D, RotateScript, EcsOnUpdate);
  EntitySetLayer(ecs, D, "box3");

  Entity E = EcsEntity(ecs, "E");
  AddComponent(ecs, E, Collider, ColliderSphere(2, true));
  AddComponent(ecs, E, Transform, TransformPosition(5, 0, -5));
  AddComponent(ecs, E, RotationSpeed, RE);
  AddComponent(ecs, E, RigidBody, RigidBodyStatic);
  AddScript(ecs, E, RotateScript, EcsOnUpdate);
  EntitySetLayer(ecs, E, "box4");

  System(ecs, DebugColliderSystem, EcsOnRender, Transform, Collider);

  EcsLoop(ecs);
  EcsFree(ecs);
  CloseWindow();

  return 0;
}
