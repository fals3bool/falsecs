# Systems

Systems are functions that process entities with specific components

## System Execution Phases

gearecs organizes systems into distinct execution phases that run in a specific order each frame.

1. `EcsOnStart` - Initialization
2. `EcsOnUpdate` - Frame-dependent game logic
3. `EcsOnLateUpdate` - Post-processing
4. `EcsOnFixedUpdate` - Physics simulation (fixed timestep)
5. `EcsOnRender` - Drawing operations
6. `EcsOnGui` - UI overlay

## Creating Systems

```C
void SystemName(ECS *ecs, Entity entity);
```

- `ecs`: Pointer to the ECS registry/world
- `entity`: The current entity being processed

Example System

```C
// Movement system that processes entities with Position and Input components
void MovementSystem(ECS *ecs, Entity entity) {
    Position *pos = GetComponent(ecs, entity, Position);
    Input *input = GetComponent(ecs, entity, Input);
    if (input->left)
        pos->x -= GetFrameTime();
    if (input->right)
        pos->x += GetFrameTime();
    if (input->up)
        pos->y -= GetFrameTime();
    if (input->down)
        pos->y += GetFrameTime();
}
```

## Registering Systems

The `System()` macro registers a system to run in a specific phase and process entities with required components:

```C
System(ecs, MovementSystem, EcsOnUpdate, Position, Input);
```

## Running Systems

```C
EcsRunSystems(ecs, EcsOnStart);
while (!WindowShouldClose()) {
    EcsRunSystems(ecs, EcsOnUpdate);
    EcsRunSystems(ecs, EcsOnLateUpdate);
    
    BeginDrawing();
    EcsRunSystems(ecs, EcsOnRender);
    EndDrawing();
}
```

## Built-in Systems

gearecs provides several built-in systems that handle common game functionality:

### Behaviour Systems
- `BehaviourStartSystem`: Runs EcsOnStart scripts from Behaviour components
- `BehaviourUpdateSystem`: Runs EcsOnUpdate scripts from Behaviour components
- `BehaviourLateSystem`: Runs EcsOnLateUpdate scripts from Behaviour components
- `BehaviourFixedSystem`: Runs EcsOnFixedUpdate scripts from Behaviour components
- `BehaviourRenderSystem`: Runs EcsOnRender scripts from Behaviour components
- `BehaviourGuiSystem`: Runs EcsOnGui scripts from Behaviour components

### Transform Systems
- `HierarchyTransformSystem`: Updates child transforms based on parent transforms
- `TransformColliderSystem`: Synchronizes collider positions with transform positions

### Physics Systems
- `CollisionSystem`: Detects and resolves collisions between entities
- `GravitySystem`: Applies gravitational forces to rigid bodies
- `PhysicsSystem`: Handles velocity, forces, and integration for rigid bodies

### Rendering Systems
- `SpriteSystem`: Renders sprite components

### Debug Systems
- `DebugColliderSystem`: Draws collider bounds and collision events

## Registering Built-in Systems

```C
// Behaviour systems
System(ecs, BehaviourStartSystem, EcsOnStart, Behaviour);
System(ecs, BehaviourUpdateSystem, EcsOnUpdate, Behaviour);
System(ecs, BehaviourLateSystem, EcsOnLateUpdate, Behaviour);
System(ecs, BehaviourFixedSystem, EcsOnFixedUpdate, Behaviour);
System(ecs, BehaviourRenderSystem, EcsOnRender, Behaviour);
System(ecs, BehaviourGuiSystem, EcsOnGui, Behaviour);

// Transform systems
System(ecs, HierarchyTransformSystem, EcsOnUpdate, Transform, Parent);
System(ecs, TransformColliderSystem, EcsOnUpdate, Transform, Collider);

// Physics systems
System(ecs, CollisionSystem, EcsOnUpdate, Transform, Collider);
System(ecs, GravitySystem, EcsOnFixedUpdate, RigidBody);
System(ecs, PhysicsSystem, EcsOnFixedUpdate, RigidBody, Transform);

// Rendering systems
System(ecs, SpriteSystem, EcsOnRender, Sprite, Transform);

// Debug systems
System(ecs, DebugColliderSystem, EcsOnRender, Collider, Transform);
```


