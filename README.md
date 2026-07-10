# Gearecs

A lightweight `Entity` `Component` `System` library for C game development, built on top of raylib.
Gearecs provides a modern, data-oriented architecture for creating efficient and scalable games.

Gearecs implements the ECS pattern, which separates data from logic:

- **Entities**: Unique identifiers representing game objects
- **Components**: Data containers (position, velocity, health, etc.)
- **Systems**: Functions that process entities with specific components

Enables highly performant, cache-friendly code with excellent modularity and reusability.

## Features

- **ECS Architecture** - Flexible, data-driven design pattern
- **Built-in Components** - Transform, RigidBody, Collider, Sprite, and more
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

void MoveScript(ECS *ecs, Entity self) {
  // Get transform component
  Transform *t = GetComponent(ecs, self, Transform);

  // Keyboard
  Vector3 d = {0};
  if (IsKeyDown(KEY_UP))
    d.y += 1;
  if (IsKeyDown(KEY_DOWN))
    d.y -= 1;
  if (IsKeyDown(KEY_LEFT))
    d.x -= 1;
  if (IsKeyDown(KEY_RIGHT))
    d.x += 1;

  // Move
  t->translation =
      Vector3Add(t->translation, Vector3Scale(d, 100 * GetFrameTime()));
}

int main(void) {
  // Raylib window and camera
  InitWindow(800, 450, "GearECS Example");

  // Registry
  ECS *world = EcsWorld();

  // Player entity
  Entity player = EcsEntity(world, "Player");
  AddComponent(world, player, Transform, TransformOrigin);
  AddComponent(world, player, Collider, ColliderCube(10, false));
  AddScript(world, player, MoveScript, EcsOnUpdate);

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





