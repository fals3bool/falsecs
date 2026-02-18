# Gearecs

A lightweight `Entity` `Component` `System` library for C game development, built on top of raylib.
Gearecs provides a modern, data-oriented architecture for creating efficient and scalable 2D games.

Gearecs implements the ECS pattern, which separates data from logic:

- **Entities**: Unique identifiers representing game objects
- **Components**: Data containers (position, velocity, health, etc.)
- **Systems**: Functions that process entities with specific components

Enables highly performant, cache-friendly code with excellent modularity and reusability.

## Features

- **ECS Architecture** - Flexible, data-driven design pattern
- **Built-in Components** - Transform2, RigidBody, Collider, Sprite, and more
- **Physics Simulation** - Complete collision detection, forces, gravity, and rigid body dynamics
- **Hierarchy Relationships** - Parent-child relationships for complex object structures
- **Scripting** - Entity-specific logic across multiple execution phases
- **Scene Management** - Automatic scene setup with camera and generic game loop
- **Multi-platform Support** - Linux, Windows, Web...
- **Raylib Integration** - Seamless rendering and input handling

## Quick Start

### CMake

`Fetch Content`

```
cmake_minimum_required(VERSION 3.25)
set(CMAKE_C_STANDARD 11)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(GEARECS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  gearecs
  GIT_REPOSITORY https://github.com/fals3bool/gearecs.git
  GIT_TAG main
)
FetchContent_MakeAvailable(gearecs)
```

`Build`

```
cmake -S . -B build -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
cmake --build build
```

```
emcmake cmake -S . -B build -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
cmake --build build
```

## Basic Example

```c
#include <gearecs.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450
#define SPEED 900

void MoveScript(ECS *ecs, Entity self) {
  RigidBody *rb = GetComponent(ecs, self, RigidBody);
  Transform2 *t = GetComponent(ecs, self, Transform2);

  // Move on mouse button pressed
  if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
    Vector2 direction = {cosf(t->rotation), sinf(t->rotation)};
    ApplyImpulse(rb, Vector2Scale(direction, SPEED));
  }

  // Look at mouse position
  Camera2D *cam = GetComponent(ecs, 0, Camera2D);
  Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), *cam);
  Vector2 mouseDirection = {mouseWorld.x - t->position.x,
                            mouseWorld.y - t->position.y};
  t->rotation = atan2f(mouseDirection.y, mouseDirection.x);
}

int main(void) {
  // Raylib window and camera
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GearECS Example");

  // Registry
  ECS *world = EcsWorld();

  // Player entity
  Entity player = EcsEntity(world, "Player");
  AddComponent(world, player, Transform2, TransformOrigin);
  AddComponent(world, player, RigidBody, RigidBodyKinematic(50, 1.5f));
  AddComponent(world, player, Collider, ColliderSolid(3, 16));
  AddScript(world, player, MoveScript, EcsOnFixedUpdate);

  // Optional (debug) system
  System(world, DebugColliderSystem, EcsOnRender, Collider);

  // Main game loop
  EcsLoop(world);

  // Cleanup
  EcsFree(world);
  CloseWindow();
  return 0;
}
```





