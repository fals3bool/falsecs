# Entity Hierarchy

Gearecs provides a hierarchical entity system with `Parent` and `Children` components, enabling parent-child relationships for complex object structures. This allows for automatic transform propagation and hierarchical state management.

## Hierarchy Components

- **Parent**: References a parent entity
- **Children**: Maintains a dynamic list of child entities

## Creating Hierarchies

While you could manually add components, use the hierarchy API for proper bidirectional relationships:

```C
// Entity B becomes child of A
AddParent(ecs, entityB, entityA);

// Equivalent function
AddChild(ecs, entityA, entityB);
```

Both functions automatically:
- Set up bidirectional parent-child references
- Update both Parent and Children components
- Handle memory management for child lists

```C
// Remove parent-child relationship
RemoveChild(world, entitA, entityB);
```

## Transform Propagation

Hierarchies work seamlessly with `Transform2` components for automatic transform propagation:

```C
// Create parent
Entity parent = EcsEntity(ecs, "Parent");
AddComponent(ecs, parent, Transform2, TransformPos(100, 100));

// Create child
Entity child = EcsEntity(ecs, "Child");
AddComponent(ecs, child, Transform2, TransformLocalPos(50, 25));

// Establish relationship
AddChild(ecs, parent, child);
```

(The `HierarchyTransformSystem` is necessary for this to work properly)

See [Systems](Systems.md) for more information about hierarchy systems.

### Local vs World Transforms

- **World transforms**: Final position/scale/rotation after applying parent transforms
- **Local transforms**: Position/scale/rotation relative to parent

```C
Transform2 *t = GetComponent(ecs, child, Transform2);
printf("Child world position: (%.1f, %.1f)\n", t->position.x, t->position.y);
printf("Child local position: (%.1f, %.1f)\n", t->localPosition.x, t->localPosition.y);
```

## Hierarchical State Management

Control entire hierarchies with single function calls:

### Active State

```C
Entity parent, child1, child2;
AddChild(ecs, parent, child1);
AddChild(ecs, parent, child2);

// Deactivate entire hierarchy
SetActive(ecs, parent, false); // Deactivates parent AND all children

// Activate specific child (doesn't reactivate parent)
SetActive(ecs, child1, true); // Only child1 becomes active
```

### Visible State

```C
// Hide entire hierarchy
SetVisible(ecs, parent, false); // Hides parent AND all children

// Show only specific child
SetVisible(ecs, child2, true); // Only child2 becomes visible
```

## Entity Destruction

### Single Entity

```C
// Remove entity but keep children alive
Destroy(ecs, parent); // Children become root entities
```

### Recursive Destruction

```C
// Remove entity and all descendants
DestroyRecursive(ecs, parent); // Entire hierarchy branch removed
```

## Iterating Hierarchies

### Direct Children Only

```C
void PrintChildInfo(ECS *ecs, Entity entity) {
    char *tag = EcsEntityData(ecs, entity)->tag;
    printf("Child: %d [%s]\n", entity, tag);
}

// Execute on direct children only
ForEachChild(ecs, parent, PrintChildInfo);
```

### All Descendants

```C
// Execute recursively on children, grandchildren, etc.
ForEachChildRecursive(ecs, parent, PrintChildInfo);
```

## Practical Examples

### Character with Equipment

```C
Entity player = EcsEntity(ecs, "Player");
Entity sword = EcsEntity(ecs, "Sword");
Entity shield = EcsEntity(ecs, "Shield");

AddComponent(ecs, player, Transform2, TransformOrigin);
AddComponent(ecs, sword, Transform2, TransformLocalPos(30, 0));
AddComponent(ecs, shield, Transform2, TransformLocalPos(-20, 0));

AddChild(ecs, player, sword);
AddChild(ecs, player, shield);
```

### UI Menu System

```C
Entity menu = EcsEntity(ecs, "Menu");
Entity title = EcsEntity(ecs, "Title");
Entity startButton = EcsEntity(ecs, "StartButton");
Entity optionsButton = EcsEntity(ecs, "OptionsButton");

AddChild(ecs, menu, title);
AddChild(ecs, menu, startButton);
AddChild(ecs, menu, optionsButton);

// Hide entire menu with one call
SetVisible(ecs, menu, false);
```

## Tips

- Alaways use `Destroy()`, `SetActive()` and `SetVisible()`. If the entity doesn't have any relationship will just modify the entity itself.
- Prefer `DestroyRecursive()` for removing entire object groups
- Use `ForEachChildRecursive()` for operations on entire subtrees
- Consider performance - deep hierarchies require more transform calculations


