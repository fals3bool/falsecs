#include <ecs/component.h>
#include <ecs/system.h>
#include <stdio.h>

Script script1;
Script script2;

int main(void) {

  Registry *r = ecs_registry();
  Entity e0 = ecs_entity(r);
  Entity e1 = ecs_entity(r);
  Entity e2 = ecs_entity(r);
  Entity e3 = ecs_entity(r);

  // FalsECS defines some components and systems:

  // ¬ - ¬ - ¬ - ¬ - ¬ - //
  // FALSECS  COMPONENTS //
  // ¬ - ¬ - ¬ - ¬ - ¬ - //

  //               //
  // RAYLIB CAMERA //
  //               //
  ecs_component(r, Camera2D);

  // recommended: use camera component with entity {id:0}.
  Camera2D cam = {0};
  cam.zoom = 1.f;
  ecs_add_obj(r, e0, Camera2D, cam);

  //                                //
  // ACTIVE AND VISIBLE PARAMETERS. //
  //                                //
  ecs_component(r, EntityData);

  ecs_add(r, e0, EntityData, ENTITYDATA_ACTIVE);
  ecs_add(r, e1, EntityData, {0});    // not active, not visible
  ecs_add(r, e2, EntityData, {0, 1}); // not active (no logic), but visible

  entity_enable(r, e0, false);
  entity_visible(r, e1, true);

  //                                              //
  // TRANSFORM WITH POSITION, SCALE AND ROTATION. //
  //                                              //
  ecs_component(r, Transform2); // uses vector2 and float rotation
  ecs_component(r, Transform);  // uses vector3 and quaternion rotation

  // falsecs doesnt define macros for 'Transform'
  // because many systems are 2d oriented.
  ecs_add(r, e1, Transform2, TRANSFORM_ZERO);
  ecs_add(r, e2, Transform2, TRANSFORM_POS(30, 20));

  Vector2 pos = {10, 10};
  Vector2 scale = {1, 1};
  ecs_add(r, e3, Transform2, {pos, scale, 30.f});

  //                                                //
  // SPRITE WITH TEXTURE, DIMENSION and COLOR TINT. //
  //                                                //
  ecs_component(r, Sprite);

  // ONLY WORKS AFTER RAYLIB: InitWindow();
  // int w = 32, h = 32;
  // Image img = GenImageColor(w, h, RED);
  // Texture tex = LoadTextureFromImage(img);
  // UnloadImage(img);
  // ecs_add(r, e1, Sprite, {tex, {0, 0, w, h}, WHITE});

  //          //
  // SCRIPTS. //
  //          //
  ecs_component(r, Behaviour);

  ecs_add(r, e1, Behaviour, {0});
  ecs_script(r, e1, script1, EcsOnStart);
  ecs_script(r, e1, script2, EcsOnUpdate);

  // scripts (OnEnable, OnDisable, OnCollisionEnter)
  ecs_add(r, e2, Behaviour, {script1, script2, script2});

  //                   //
  // POLYGON COLLIDER. //
  //                   //
  ecs_component(r, Collider);

  // 5 vertices, r=12, offset=(+0, +0), solid
  Collider col = collider_create(5, 12, 0, (Vector2){0, 0}, 1);
  col = collider_solid(5, 12);  // solid collider - hit others
  col = collider_hollow(5, 12); // hollow collider - overlaps others
  ecs_add_obj(r, e1, Collider, col);

  //          //
  // PHYSICS. //
  //          //
  ecs_component(r, RigidBody);

  // mass = 20g // dynamic = gravity
  ecs_add(r, e1, RigidBody, RIGIDBODY_DYNAMIC(20, 0));

  // mass = 20g, damping = 0.7 // static = no gravity
  ecs_add(r, e2, RigidBody, RIGIDBODY_STATIC(20, 0.7f));

  // 30 kg // static = 0 (no gravity).
  ecs_add(r, e3, RigidBody, {30});

  RigidBody *rb = ecs_get(r, e2, RigidBody);
  rb_apply_force(rb, (Vector2){20, 0});   // changes acceleration
  rb_apply_impulse(rb, (Vector2){0, 10}); // changes speed

  // ¬- ¬ - ¬ - ¬- ¬ - ¬ - ¬- //
  // FALSECS OPTIONAL SYSTEMS //
  // ¬- ¬ - ¬ - ¬- ¬ - ¬ - ¬- //

  // draw collider's vertices and edges.
  ecs_system(r, EcsOnRender, ecs_debug_collider_system, Transform2, Collider);

  ecs_run(r, EcsOnStart);
  ecs_run(r, EcsOnUpdate);

  entity_enable(r, e2, true);
  entity_enable(r, e2, false);

  // thats it...
  ecs_registry_free(r);

  // NO NEED TO DO THIS WHEN USING SCENE MANAGER
  // SCENE MANAGER DEALS WITH REGISTRY, COMPONENTS AND DEFAULT SYSTEMS.

  return 0;
}

void script1d(Registry *r, Entity self) {
  printf("script1 on start layer or entity was enabled\n");
}
Script script1 = script1d;

void script2d(Registry *r, Entity self) {
  printf("script2 on update layer or entity was disabled\n");
}
Script script2 = script2d;
