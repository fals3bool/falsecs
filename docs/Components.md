# Components

Components are the data containers. They hold all the state and properties for entities, while systems operate on this data.

## Custom Components

Components are defined as plain C structs:

```C
typedef struct {
    float health;
    float max_health;
    float speed;
    uint32_t experience_points;
    uint8_t level;
} PlayerStats;

typedef struct {
    EcsID *items;
    uint32_t *quantities;
    uint8_t capacity;
    uint8_t count;
} Inventory;
```

Before using a component, it must be registered with the ECS registry:

```C
// Register a simple component
Component(world, PlayerStats);

// Register a component with dynamic memory (requires destructor)
ComponentDynamic(world, Inventory, InventoryDestructor);
```

## Component CRUD

### Creating Components

```C
Entity player = EcsEntity(world, "Player");

// Add component with initializer list
AddComponent(world, player, PlayerStats, {50.0f, 50.0f, 3.0f, 0, 2});

// Add component using existing variable
PlayerStats defaultStats = {30.0f, 30.0f, 2.0f, 0, 1};
AddComponent(world, player, PlayerStats, defaultStats);
```

### Reading Components

```C
// Get component pointer (returns NULL if entity doesn't have the component)
PlayerStats *stats = GetComponent(world, player, PlayerStats);

if (stats) {
    printf("Player health: %.1f/%.1f\n", stats->health, stats->max_health);
    printf("Player level: %d\n", stats->level);
    
    // Modify component data
    stats->health -= 10.0f;
    stats->experience_points += 25;
}
```

### Updating Components

Components are updated by modifying their data directly:

```C
void LevelUp() {
    PlayerStats *stats = GetComponent(world, player, PlayerStats);
    if (stats && stats->experience_points >= 100) {
        stats->level++;
        stats->experience_points -= 100;
        stats->max_health += 20.0f;
        stats->health = stats->max_health;
        stats->speed += 0.5f;
    }
}
```

### Removing Components

```c
RemoveComponent(world, player, PlayerStats);
```

### Component Templates

Create reusable component templates for consistent initialization:

```C
#define WarriorStats {150.f, 150.f, 3.5f, 0, 1}
#define MageStats {80.f, 80.f, 4.f, 0, 1}

Entity warrior = EcsEntity(world, "Warrior");
AddComponent(world, warrior, PlayerStats, WarriorStats);

Entity mage = EcsEntity(world, "Mage");
AddComponent(world, mage, PlayerStats, MageStats);
```

## Dynamic Components

Components that allocate memory require special handling to prevent memory leaks.

### When to Use Dynamic Components

Use `ComponentDynamic` when your component:
- Allocates memory with malloc/calloc
- Contains pointers to heap-allocated data
- Needs cleanup when the component or entity is destroyed

### Dynamic Component Example

```C
typedef struct {
    EcsID *items;
    uint32_t *quantities;
    uint8_t capacity;
    uint8_t count;
} Inventory;

void InventoryDestructor(ECS *ecs, void *component) {
    Inventory *inv = (Inventory *)component;
    if (inv->items)
        free(inv->items);
    if (inv->quantities)
        free(inv->quantities);

    // Note: Don't free the component itself - gearecs handles this
    // free(inv); // >:(
}

ComponentDynamic(world, Inventory, InventoryDestructor);
```

## Built-in Components

Gearecs provides several built-in components for common game functionality:

`Transform`
- Handles position (translation), scale, and rotation (Quaternion)
- Supports parent-child relationships
- Automatically updated by hierarchy systems

`RigidBody`
- Physics simulation properties
- Integrates with physics systems
- Supports different body types

`Collider`
- Convex collision detection
- Collision filtering through entity layers

`CollisionListener`
- Collision event handlers

`Sprite`
- 2D rendering with raylib

`Behaviour`
- Entity-specific logic (scripting)

`Parent / Children`
- Entity hierarchy
- Allows hierarchy transformations and state control.

