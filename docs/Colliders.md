# Colliders

Gearecs provides convex polyhedron colliders with GJK (Gilbert–Johnson–Keerthi) + EPA (Expanding Polytope Algorithm) collision detection and response.

## Collider Types

There are two types of colliders:

- **Solid**: Block movement and generate collision responses. Used for walls, floors, obstacles.
- **Trigger**: Detect overlap without blocking. Used for area effects, pickups, detection zones.

## Creating Colliders

### Cubes

Cubes only need the distance between vertices and a solid boolean flag.

```C
// Solid cube
AddComponent(ecs, entity, Collider, ColliderCube(5, true)); // edges' length = 5

// Trigger cube
AddComponent(ecs, entity, Collider, ColliderCube(6, false));
```

## Collision Requirements

For solid colliders to interact physically, entities need both `Collider` and `RigidBody` components:

```C
// Solid physics object
Entity player = EcsEntity(ecs, "Player");
AddComponent(ecs, player, Transform, TransformOrigin);
AddComponent(ecs, player, Collider, ColliderCube(5, true)); // solid
AddComponent(ecs, player, RigidBody, RigidBodyDynamic(80, 1.52f)); // Warning: dynamic rigidbodies have gravity

// Static wall (no RigidBody needed for static geometry)
Entity wall = EcsEntity(ecs, "Wall");
AddComponent(ecs, wall, Transform, TransformPosition(100, 0, 0));
AddComponent(ecs, wall, Collider, ColliderCube(6, true)); // solid
```

## Collision Events

Handle collisions with the `CollisionListener` component:

```C
void OnCollision(ECS *ecs, CollisionEvent *event) {
    printf("Entity %d collided with %d!\n", event->self, event->other);
    
    // Access collision details
    Vector2 normal = event->collision.normal;
    float penetration = event->collision.distance;
    
    // Apply damage, trigger effects, etc.
}

Entity player = EcsEntity(ecs, "Player");
AddComponent(ecs, player, CollisionListener, {OnCollision}); // handler only
```

## Collision Systems

GearECS provides built-in collision systems:
- `TransformColliderSystem` - Updates collider positions based on transforms
- `CollisionSystem` - Detects and resolves collisions  
- `DebugColliderSystem` - Renders collider debug information

See [Systems](Systems.md) for information about registering these systems.

## Tips

- Use triggers for non-physical interactions
- Combine with layer filtering for performance
- Debug with `DebugColliderSystem` to verify collision shapes



