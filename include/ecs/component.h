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

// Same data structure, used for hierarchy transformation.
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
 * @brief Supported collider geometry types.
 *
 * Each type corresponds to a shape struct that holds the specific
 * geometric data for collision detection.
 */
typedef enum {
  Box,     ///< Axis-aligned box (BoxShape)
  Sphere,  ///< Bounding sphere (SphereShape)
  Capsule, ///< Capsule shape (CapsuleShape)
  Convex,  ///< Convex hull from vertices (ConvexShape)
} ColliderShape;

/**
 * @brief Axis-aligned box collider shape.
 *
 * Defined by a center point and box bounds.
 */
typedef struct {
  Vector3 center; ///< Center of the box
  float width;    ///< Box width along X axis
  float height;   ///< Box height along Y axis
  float length;   ///< Box length along Z axis
} BoxShape;

/**
 * @brief Bounding sphere collider shape.
 *
 * Simplest collision shape; defined by center and radius.
 */
typedef struct {
  Vector3 center; ///< Center of the sphere
  float radius;   ///< Radius of the sphere
} SphereShape;

/**
 * @brief Capsule collider shape.
 *
 * A cylinder with hemispherical ends, defined by two endpoints
 * (top and bottom) and a radius.
 */
typedef struct {
  Vector3 top;    ///< Center of the top hemisphere
  Vector3 bottom; ///< Center of the bottom hemisphere
  float radius;   ///< Radius of the capsule
  float height;   ///< Height of the cylindrical section
} CapsuleShape;

/**
 * @brief Convex hull collider shape.
 *
 * Arbitrary convex polygon defined by a set of vertices.
 * The vertex arrays are heap-allocated.
 */
typedef struct {
  Vector3 *vx;      ///< Array of vertices in world space
  Vector3 *md;      ///< Axis-aligned vertices at origin (model data)
  uint8_t vertices; ///< Number of vertices
} ConvexShape;

/**
 * @brief 3D Collider
 *
 * Uses shapes to store geometric data, which is heap-allocated.
 *
 * Supports both solid (block movement) and trigger colliders
 * (detect overlap without blocking).
 * Collision filtering is handled through entity layers managed
 * by the registry.
 */
typedef struct {
  ColliderShape type; ///< shape type
  void *shape;        ///< Shape data
  bool solid;         ///< Whether the collider blocks movement or acts as a trigger.
  bool overlap;       ///< Collision overlap flag
} Collider;

/**
 * @brief Creates a box collider.
 *
 * @param width Width along the X axis
 * @param height Height along the Y axis
 * @param length Length along the Z axis
 * @param solid Whether the collider is solid or acts as a trigger
 * @return A box Collider instance
 *
 * @see ColliderDestructor()
 */
Collider ColliderBox(float width, float height, float length, bool solid);

/**
 * @brief Creates a capsule collider.
 *
 * @param radius Radius of the capsule
 * @param height Height of the cylindrical section
 * @param solid Whether the collider is solid or acts as a trigger
 * @return A capsule Collider instance
 *
 * @see ColliderDestructor()
 */
Collider ColliderCapsule(float radius, float height, bool solid);

/**
 * @brief Creates a sphere collider.
 *
 * @param radius Radius of the sphere
 * @param solid Whether the collider is solid or acts as a trigger
 * @return A sphere Collider instance
 *
 * @see ColliderDestructor()
 */
Collider ColliderSphere(float radius, bool solid);

/**
 * @brief Creates a convex hull collider from a vertex array.
 *
 * The vertex array is copied internally; the caller retains ownership
 * of the original data.
 *
 * @param model Array of vertices defining the convex hull
 * @param vertices Number of vertices (max 255)
 * @param solid Whether the collider is solid or acts as a trigger
 * @return A convex hull Collider instance
 *
 * @see ColliderDestructor()
 */
Collider ColliderConvex(Vector3 *model, uint8_t vertices, bool solid);

/**
 * @brief Destructor for Collider component.
 *
 * Automatically frees collider's shape when collider is removed
 * or entity is destroyed. Registered with ComponentDynamic().
 *
 * @param ptr Pointer to Collider instance
 */
void ColliderDestructor(void *ptr);

/**
 * @brief Collision data.
 *
 * Contains information about collision normal and penetration depth
 * for collision resolution and response.
 */
typedef struct {
  Vector3 normal; ///< Collision normal vector (direction of separation)
  float distance; ///< Penetration depth (positive = overlapping)
} Collision;

/**
 * @brief Collision event between two entities.
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
 * @brief Collision handler function type.
 *
 * Function called when collision occurs.
 *
 * @param ecs Registry containing the entities
 * @param event Collision event data
 */
typedef void (*CollisionHandler)(ECS *, CollisionEvent *);

/**
 * @brief Component for receiving collision events.
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
#define RigidBodyCreate(mass, damping, type)                                             \
  {(mass > 0) ? mass : INFINITY,                                                         \
   (mass > 0) ? 1.f / mass : 0,                                                          \
   damping,                                                                              \
   type,                                                                                 \
   (type == BodyDynamic) ? true : false,                                                 \
   {0, 0, 0},                                                                            \
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
#define RigidBodyStatic RigidBodyCreate(INFINITY, 0, BodyStatic)

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
#define RigidBodyDynamic(mass, damping) RigidBodyCreate(mass, damping, BodyDynamic)

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
#define RigidBodyKinematic(mass, damping) RigidBodyCreate(mass, damping, BodyKinematic)

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
 * Impulse immediately changes velocity.
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
 * Wraps a raylib texture with source rectangle and tint for
 * sprite-sheet support and color modulation.
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
 * @param phase System phase to run script in
 *
 * Example: AddScript(world, player, PlayerUpdate, EcsOnUpdate);
 */
void AddScript(ECS *ecs, Entity e, Script s, EcsPhase phase);

// ########### //
//  HIERARCHY  //
// ########### //

/**
 * @brief Hierarchical relationship component.
 *
 * Represents an entity within a tree hierarchy using a doubly linked list
 * representation for siblings. Each entity can have at most one parent
 * and any number of children.
 *
 * Children of the same parent are connected through the
 * @ref leftSibling and @ref rightSibling links, while the parent stores
 * a reference only to its first child.
 *
 * Example:
 * @code
 * AddComponent(world, entityA, Hierarchy, {InvalidID, InvalidID, InvalidID, entityB});
 * @endcode
 *
 * @see InvalidID for sentinel value
 * @see EntitySetParent() for automatic attachment
 * @see EntityAddChild() for automatic attachment
 */
typedef struct {
  Entity parent;       ///< Entity parent. InvalidID if it has no parent.
  Entity leftSibling;  ///< Previous sibling. InvalidID for first child.
  Entity rightSibling; ///< Next sibling. InvalidID for last child.
  Entity firstChild;   ///< First child. InvalidID if it has no children.
} Hierarchy;

/**
 * @brief Attach parent and child entities within a hierarchical relationship.
 *
 * Removes the child entity from its current parent (if any) and adds it
 * as a child of the specified parent. Automatically updates hierarchy
 * components on both entities.
 *
 * @param ecs Registry containing the entities
 * @param parent Parent entity
 * @param child Child entity
 * @return false if it is not possible, true otherwise
 */
bool HierarchyAttach(ECS *ecs, Entity parent, Entity child);

/**
 * @brief Removes the parent from the entity in the hierarchy.
 *
 * Removes the entity from its current parent and automatically updates
 * hierarchy components on both entities.
 *
 * @param ecs Registry containing the entities
 * @param e Child entity
 */
void HierarchyDetach(ECS *ecs, Entity e);

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
 * @see EntityDestroyRecursive() to destroy with all descendants
 * @see EcsEntityFree() for basic entity destruction
 */
void EntityDestroy(ECS *ecs, Entity e);

/**
 * Destroys an entity and all its descendants.
 *
 * Recursively destroys the entity and all entities in its subtree.
 * Use this when you want to remove an entire hierarchy branch.
 *
 * @param ecs Registry containing the entity
 * @param e Entity to destroy
 *
 * @see EntityDestroy() to destroy without descendants
 * @see EcsEntityFree() for basic entity destruction
 */
void EntityDestroyRecursive(ECS *ecs, Entity e);

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
 * @see HierarchyForEachChildRecursive() for all descendants
 */
void HierarchyForEachChild(ECS *ecs, Entity e, Script s);

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
 * @see HierarchyForEachChild() for direct children only
 */
void HierarchyForEachChildRecursive(ECS *ecs, Entity e, Script s);

/**
 * Sets whether an entity (and its children) is active for system processing.
 *
 * @param ecs Registry containing the entity
 * @param e Entity to modify
 * @param active true for activated, false for deactivated
 *
 * @see EntitySetActiveSelf() to ignore entity children
 */
void EntitySetActive(ECS *ecs, Entity e, bool active);

/**
 * Sets whether an entity (and its children) is visible for rendering.
 *
 * @param ecs Registry containing the entity
 * @param e Entity to modify
 * @param visible true for visible, false for hidden
 *
 * @see EntitySetVisibleSelf() to ignore entity children
 */
void EntitySetVisible(ECS *ecs, Entity e, bool visible);

#endif
