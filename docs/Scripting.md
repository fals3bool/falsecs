# Scripting

Entities can have custom scripts that run during different execution phases using the `Behaviour` component. This enables per-entity logic without creating separate systems.

## Behaviour Component

The `Behaviour` component provides:
- Scripts for each execution phase (EcsOnStart, EcsOnUpdate, etc.)
- `OnEnable` script called when entity becomes active
- `OnDisable` script called when entity becomes inactive

## Creating Scripts

Scripts follow the standard ECS function signature:

```C
void PlayerStart(ECS *ecs, Entity self) {
    printf("Player spawned!\n");
    // Initialization logic here
}

void PlayerUpdate(ECS *ecs, Entity self) {
    Transform *transform = GetComponent(ecs, self, Transform);
    
    // Movement logic
    if (IsKeyDown(KEY_LEFT))
        transform->position.x -= GetFrameTime() * 100;
    if (IsKeyDown(KEY_RIGHT))
        transform->position.x += GetFrameTime() * 100;
}
```

## Adding Scripts

Use `AddScript()` to add scripts to entities. The `Behaviour` component is added automatically if needed.

```C
// Create entity
Entity player = EcsEntity(ecs, "Player");
AddComponent(ecs, player, Transform, TransformOrigin);

// Add scripts for different phases
AddScript(ecs, player, PlayerStart, EcsOnStart);
AddScript(ecs, player, PlayerUpdate, EcsOnUpdate);
```

## Tips

- Use scripts for entity-specific logic (player controls, AI behavior)
- Use systems for general logic that processes many entities (movement, rendering)
- Scripts run only on active/visible entities depending on the phase

See [Systems](Systems.md) for information about registering built-in Behaviour systems



