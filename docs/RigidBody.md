# RigidBody

Rigid bodies are used for physics simulation, providing realistic motion, forces, and collision response.

## Body Types

There are three types of rigid bodies:

- `BodyStatic`: Infinite mass and no damping, but can collide with dynamic bodies. Useful for walls, floors, platforms.
- `BodyDynamic`: Full physics simulation and are affected by forces, gravity, and collisions. Useful for players, enemies, moveable objects.
- `BodyKinematic`: Moved manually, affect dynamic bodies but aren't affected by forces or gravity. Useful for moving platforms, elevators.

## Creating Rigid Bodies

```C
// Static - infinite mass, no damping
AddComponent(ecs, entity, RigidBody, RigidBodyStatic);

// Dynamic - mass: 50, damping: 1.5
AddComponent(ecs, entity, RigidBody, RigidBodyDynamic(50, 1.5f));

// Kinematic - mass: 100, damping: 2.5
AddComponent(ecs, entity, RigidBody, RigidBodyKinematic(100, 2.5f));
```

## Forces and Impulses

### Force vs Impulse

- **Force**: Changes object acceleration over time. Use for continuous forces like gravity, thrusters, springs.
- **Impulse**: Changes object velocity immediately. Use for sudden impacts, explosions, jumps.

### Applying Forces

```C
// Apply continuous force (accumulated)
Vector3 force = {0, -9.8f, 0}; // Gravity
RigidBody *rb = GetComponent(ecs, entity, RigidBody);
ApplyForce(rb, force);

// Apply instantaneous impulse
Vector3 impulse = {0, 15.0f, 0}; // Jump force
ApplyImpulse(rb, impulse);
```

## Physics Properties

RigidBody components include these properties:

```C
typedef struct {
  float mass;    // Object mass (g), 0 or INFINITY for static objects
  float invmass; // Inverse mass (1/mass), 0 for static objects
  float damping; // Velocity damping factor (0 = no damping)
  BodyType type; // Physics behavior type
  bool gravity;  // Whether gravity affects this body
  Vector3 speed; // Current velocity (units/second)
  Vector3 acc;   // Current acceleration (units/second²)
} RigidBody;
```

## Physics Integration

Rigid bodies integrate with built-in physics systems:
- `GravitySystem` applies gravitational force to bodies with gravity enabled
- `PhysicsSystem` integrates forces, applies damping, and updates positions
- Physics runs in `EcsOnFixedUpdate` for consistent simulation regardless of frame rate

See [Systems](Systems.md) for information about registering built-in physics systems.

## Tips

- Use fixed timestep physics (`EcsOnFixedUpdate`) for consistent behavior
- Redefine `FIXED_UPDATES` to change the fixed update phase timestep
- Set appropriate mass values: heavier objects accelerate more slowly
- Use damping to prevent infinite motion
- Combine with `Collider` components for collision response
- Static bodies don't need mass or damping calculations



