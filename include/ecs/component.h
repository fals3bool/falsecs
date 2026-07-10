#ifndef ECS_COMPONENT_H
#define ECS_COMPONENT_H

/**
 * @file component.h
 * @brief Built-in component definitions for gearecs ECS library
 *
 * This header provides a comprehensive set of ready-to-use components
 * for common game development needs. Including:
 *
 * - Entity metadata
 * - Entity hierarchy management
 * - Transforms with hierarchical support
 * - Physics components (RigidBody, Collider)
 * - Rendering components (Sprite)
 * - Scripting and behavior system
 */

#include <ecs/registry.h>

#include <raylib.h>
#include <raymath.h>

// ########### //
//  TRANSFORM  //
// ########### //

typedef Transform LocalTransform;

/**
 * @brief Creates a zero-initialized transform.
 *
 * Position at origin, unit scale and rotation.
 *
 * @return Transform initializer
 *
 * Example: AddComponent(world, e, Transform, TransformOrigin);
 */
#define TransformOrigin {{0, 0, 0}, {0, 0, 0, 1}, {1, 1, 1}}

/**
 * @brief Creates a transform with specific world position.
 *
 * Unit scale and rotation.
 *
 * @param x X coordinate in world space
 * @param y Y coordinate in world space
 * @param z Z coordinate in world space
 * @return Transform initializer
 *
 * Example: AddComponent(world, e, Transform, TransformPos(100.0f, 200.0f,
 * 150.0f));
 */
#define TransformPosition(x, y, z) {{x, y, z}, {0, 0, 0, 1}, {1, 1, 1}}

// ########## //
//  COLLIDER  //
// ########## //

/**
 * 3D Collider
 *
 * Supports both solid colliders (block movement) and trigger colliders
 * (detect overlap without blocking). Uses polygon-based collision with
 * configurable vertices. Collision filtering is handled through entity layers
 * managed by the registry.
 */
typedef struct {
  Vector3 *vx;      ///< Array of vertices
  Vector3 *md;      ///< Axis-aligned vertices located at origin (model)
  uint8_t *edge;    ///< Shape edges with vertex index, used for rendering.
  uint8_t vertices; ///< Number of vertices
  uint8_t edges;    ///< Number of edges
  bool solid;       ///< true for solid, false for trigger
  bool overlap;     ///< Collision overlap flag
} Collider;

/**
 * Destructor for Collider component.
 *
 * Automatically frees vertex array memory when collider is removed
 * or entity is destroyed. Registered with ComponentDynamic().
 *
 * @param self Pointer to Collider instance
 */
void ColliderDestructor(void *self);

/**
 * Creates a cube collider
 *
 * @param size distance between adjacent vertices
 * @param solid true for solid, false for trigger
 * @return Collider instace
 *
 * @see ColliderDestructor
 */
Collider ColliderCube(float size, bool solid);

/**
 * Collision data.
 *
 * Contains information about collision normal and penetration depth
 * for collision resolution and response.
 */
typedef struct {
  Vector3 normal; ///< Collision normal vector (direction of separation)
  float distance; ///< Penetration depth (positive = overlapping)
} Collision;

/**
 * Collision event between two entities.
 *
 * Passed to collision handlers with information about
 * the colliding entities and collision details.
 */
typedef struct {
  Entity self;         ///< Entity receiving the collision event
  Entity other;        ///< Entity being collided with
  Collision collision; ///< Collision data
} CollisionEvent;

/**
 * Collision handler function type.
 *
 * Function called when collision occurs.
 *
 * @param ecs Registry containing the entities
 * @param event Collision event data
 */
typedef void (*CollisionHandler)(ECS *, CollisionEvent *);

/**
 * Component for receiving collision events.
 */
typedef struct {
  // CollisionHandler OnCollisionEnter;  ///< TODO: Collision start handler
  // CollisionHandler OnCollisionStay;   ///< TODO: Collision continue handler
  // CollisionHandler OnCollisionExit;   ///< TODO: Collision end handler
  CollisionHandler OnCollision; ///< Current collision handler
} CollisionListener;

// ############ //
//  RIGID BODY  //
// ############ //

/**
 * Rigid body physics simulation types.
 *
 * Determines how an entity participates in physics simulation:
 * - Static: Immovable, infinite mass objects (walls, floors)
 * - Dynamic: Full physics simulation, affected by forces (players, objects)
 * - Kinematic: Moved manually
 */
typedef enum {
  BodyStatic = 0, ///< Immovable objects with infinite mass
  BodyDynamic,    ///< Full physics simulation
  BodyKinematic   ///< Manually controlled objects
} BodyType;

/**
 * 3D rigid body physics component.
 *
 * Provides realistic physics simulation including forces, impulses,
 * mass, damping, and gravity support. Integrates with collider
 * components for collision response.
 */
typedef struct {
  float mass;    ///< Object mass (g), 0 or INFINITY for static objects
  float invmass; ///< Inverse mass (1/mass), 0 for static objects
  float damping; ///< Velocity damping factor (0 = no damping)
  BodyType type; ///< Physics behavior type
  bool gravity;  ///< Whether gravity affects this body
  Vector3 speed; ///< Current velocity (units/second)
  Vector3 acc;   ///< Current acceleration (units/second²)
} RigidBody;

/**
 * Creates a rigid body with specified parameters.
 *
 * Low-level macro. Automatically handles mass/inverse mass calculation and
 * gravity setting based on type.
 *
 * @param mass Mass value (use 0 or INFINITY for static objects)
 * @param damping Damping factor
 * @param type BodyType enum value
 * @return RigidBody initializer
 */
#define RigidBodyCreate(mass, damping, type)                                   \
  {(mass > 0) ? mass : INFINITY,                                               \
   (mass > 0) ? 1.f / mass : 0,                                                \
   damping,                                                                    \
   type,                                                                       \
   (type == BodyDynamic) ? true : false,                                       \
   {0, 0, 0},                                                                  \
   {0, 0, 0}}

/**
 * Creates a static rigid body.
 *
 * Static bodies have infinite mass and don't move, but can collide
 * with dynamic bodies.
 *
 * @return RigidBody initializer
 *
 * Example: AddComponent(world, e, RigidBody, RigidBodyStatic);
 */
#define RigidBodyStatic RigidBodyCreate(0, 0, BodyStatic)

/**
 * Creates a dynamic rigid body.
 *
 * Dynamic bodies participate fully in physics simulation and are
 * affected by forces, gravity, and collisions. Perfect for players,
 * enemies, moveable objects.
 *
 * @param mass Object mass
 * @param damping Velocity damping factor (0-1..)
 * @return RigidBody initializer
 *
 * Example: AddComponent(world, e, RigidBody, RigidBodyDynamic(300.f, 1.52f));
 */
#define RigidBodyDynamic(mass, damping)                                        \
  RigidBodyCreate(mass, damping, BodyDynamic)

/**
 * Creates a kinematic rigid body.
 *
 * Kinematic bodies are moved manually (direct velocity control) and
 * affect dynamic bodies but aren't affected by forces or gravity.
 * Perfect for moving platforms, elevators, controlled objects.
 *
 * @param mass Object mass (typically 0)
 * @param damping Damping factor
 * @return RigidBody initializer
 *
 * Example: AddComponent(world, e, RigidBody, RigidBodyKinematic(0, 0.95f));
 */
#define RigidBodyKinematic(mass, damping)                                      \
  RigidBodyCreate(mass, damping, BodyKinematic)

/**
 * Applies a continuous force to a rigid body.
 *
 * Force is accumulated and applied during physics integration.
 * Use for constant forces like gravity, thrusters, springs.
 *
 * @param rb RigidBody to apply force to
 * @param force Force vector
 *
 * @note Force is accumulated, call before physics update
 */
void ApplyForce(RigidBody *rb, Vector3 force);

/**
 * Applies an instantaneous impulse to a rigid body.
 *
 * Impulse immediately changes velocity regardless of mass.
 * Use for sudden impacts, explosions, jumps.
 *
 * @param rb RigidBody to apply impulse to
 * @param impulse Impulse vector
 *
 * @note Impulse is applied immediately
 */
void ApplyImpulse(RigidBody *rb, Vector3 impulse);

/**
 * Applies velocity damping to a rigid body.
 *
 * Reduces velocity over time to simulate air resistance or friction.
 * Automatically called by the physics system.
 *
 * @param rb RigidBody to apply damping to
 *
 * @note Called internally by PhysicsSystem()
 */
void ApplyDamping(RigidBody *rb);

// ######## //
//  SPRITE  //
// ######## //

/**
 * 2D sprite rendering component.
 *
 */
typedef struct {
  Texture tex;   ///< Raylib texture to render
  Rectangle src; ///< Source rectangle within texture (for sprite sheets)
  Color tint;    ///< Color tint for rendering
} Sprite;

// ########### //
//  BEHAVIOUR  //
// ########### //

/**
 * Component for entity scripting and behavior management.
 *
 * Allows entities to have custom scripts that run at different
 * phases of the game loop. Supports enable/disable events.
 */
typedef struct {
  Script OnEnable;                ///< Script called when entity is enabled
  Script OnDisable;               ///< Script called when entity is disabled
  Script scripts[EcsTotalPhases]; ///< Scripts for each ecs layer
} Behaviour;

/**
 * Adds a script to an entity's Behaviour component.
 *
 * Automatically adds Behaviour component if entity doesn't have one.
 * The script will be called during the specified ecs layer.
 *
 * @param ecs Registry containing the entity
 * @param e Entity to add script to
 * @param s Script to add
 * @param ly System layer to run script in
 *
 * Example: AddScript(world, player, PlayerUpdate, EcsOnUpdate);
 */
void AddScript(ECS *ecs, Entity e, Script s, EcsPhase phase);

// ########### //
//  HIERARCHY  //
// ########### //

/**
 * Parent component for entity hierarchy.
 *
 * Links an entity to its parent in the hierarchy. Used with Transform
 * to implement hierarchical transformations where child entities
 * inherit parent transformations.
 */
typedef struct {
  Entity entity; ///< Parent entity ID
} Parent;

/**
 * Children component for entity hierarchy.
 *
 * Maintains a list of child entities. Used internally by the hierarchy
 * system to manage parent-child relationships and enable operations
 * on all children of an entity.
 *
 * @note Using this component's fields is deprecated. Use foreach functions
 * instead.
 *
 * @see ForEachChild()
 * @see ForEachChildRecursive()
 */
typedef struct {
  Entity *list;
  Entity count;
  Entity allocated;
} Children;

/**
 * Destructor for Children component.
 *
 * Automatically frees child list array memory when Children component
 * is removed or entity is destroyed. Registered with ComponentDynamic().
 *
 * @param self Pointer to Children instance
 */
void ChildrenDestructor(void *self);

/**
 * Sets a parent for an entity in the hierarchy.
 *
 * Removes the entity from its current parent (if any) and adds it
 * as a child of the specified parent. Automatically updates hierarchy
 * components on both entities.
 *
 * @param ecs Registry containing the entities
 * @param e Child entity
 * @param p Parent entity
 */
void AddParent(ECS *ecs, Entity e, Entity p);

/**
 * Adds a child entity to another entity.
 *
 * Removes the child from its current parent (if any) and adds it as
 * a child of the specified parent.
 *
 * @param ecs Registry containing the entities
 * @param e Parent entity
 * @param c Child entity to add
 */
void AddChild(ECS *ecs, Entity e, Entity c);

/**
 * Removes a child from its parent.
 *
 * Removes the parent-child relationship between the specified entities.
 * The child becomes a root entity (no parent).
 *
 * @param ecs Registry containing the entities
 * @param e Parent entity
 * @param c Child entity to remove
 */
void RemoveChild(ECS *ecs, Entity e, Entity c);

/**
 * Destroys an entity and removes it from hierarchy.
 *
 * Removes the entity from its parent (if any), but keeps children alive.
 * Children become root entities. Use DestroyRecursive() to destroy
 * the entire hierarchy branch.
 *
 * @param ecs Registry containing the entity
 * @param e Entity to destroy
 *
 * @see DestroyRecursive() to destroy with all descendants
 * @see EcsEntityFree() for basic entity destruction
 */
void Destroy(ECS *ecs, Entity e);

/**
 * Destroys an entity and all its descendants.
 *
 * Recursively destroys the entity and all entities in its subtree.
 * Use this when you want to remove an entire hierarchy branch.
 *
 * @param ecs Registry containing the entity
 * @param e Entity to destroy
 *
 * @see Destroy() to destroy without descendants
 * @see EcsEntityFree() for basic entity destruction
 */
void DestroyRecursive(ECS *ecs, Entity e);

/**
 * Executes a script on all direct children of an entity.
 *
 * Iterates over all immediate children and calls the specified script.
 * Does not include grandchildren or deeper descendants.
 *
 * @param ecs Registry containing the entity
 * @param e Parent entity
 * @param s Script to execute on each child
 *
 * @see ForEachChildRecursive() for all descendants
 */
void ForEachChild(ECS *ecs, Entity e, Script s);

/**
 * Executes a script on all descendants of an entity.
 *
 * Recursively iterates over all descendants (children, grandchildren,
 * etc.) and calls the specified script on each one.
 *
 * @param ecs Registry containing the entity
 * @param e Root entity
 * @param s Script to execute on each descendant
 *
 * @see ForEachChild() for direct children only
 */
void ForEachChildRecursive(ECS *ecs, Entity e, Script s);

/**
 * Sets whether an entity (and its children) is active for system processing.
 *
 * @param ecs Registry containing the entity
 * @param e Entity to modify
 * @param active true for activated, false for deactivated
 *
 * @see EntitySetActive() to ignore entity children
 */
void SetActive(ECS *ecs, Entity e, bool active);

/**
 * Sets whether an entity (and its children) is visible for rendering.
 *
 * @param ecs Registry containing the entity
 * @param e Entity to modify
 * @param visible true for visible, false for hidden
 *
 * @see EntitySetVisible() to ignore entity children
 */
void SetVisible(ECS *ecs, Entity e, bool visible);

#endif
