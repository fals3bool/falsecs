# World

The World is a high-level wrapper that provides a complete game development environment. It builds upon the Registry by:

- Automatically registering all built-in components (Transform, RigidBody, Collider, Sprite, etc.)
- Registering all built-in systems (physics, collision, rendering, etc.)
- Creating a camera entity with proper components for rendering
- Providing a complete, optimized game loop with `EcsLoop()`

## Registry vs World

- **Registry**: Low-level ECS management, manual system registration and execution
- **World**: High-level wrapper with pre-registered components and systems and automatic game loop

Use Registry when you need:
- Full control over component/system registration
- Custom game loop implementation

Use World when you need:
- Quick setup with built-in functionality
- Standard game loop structure 
- Ready-to-use components and systems 

## World Usage

```C
#include <gearecs.h>


int main(void) {
    InitWindow(800, 450, "Getting Started with Gearecs");

    ECS *world = EcsWorld();
    
    // Your game logic here
    
    // Run the game loop (blocks until window should close)
    EcsLoop(world);
    
    // Cleanup
    EcsFree(world);
    CloseWindow();
    return 0;
}
```

### World Camera

```C
Transform *transform = GetComponent(world, player, Transform);
Camera *camera = WorldMainCamera(world);
camera.target = transform.position;
```

### Custom FixedUpdate

Define `FIXED_UPDATES` before including the gearecs header.

```C
#define FIXED_UPDATES 144

#include <gearecs.h>

void foo() {
    ECS *world = EcsWorld();
    EcsLoop(world); // EcsOnFixedUpdate 144 times per second
}
```
