#ifndef GEARECS_WORLD
#define GEARECS_WORLD

/**
 * @file world.h
 * @brief High-level ECS world management for gearecs library
 *
 * This header provides convenience functions for creating and managing
 * a complete ECS world with pre-configured systems and components.
 * It offers a simplified interface for common game development workflows
 * by handling the setup of the ECS registry, camera, and main game loop.
 *
 * The world management functions include:
 * - Pre-configured ECS registry creation with camera integration
 * - Automatic system registration for built-in functionality
 * - Main game loop handling with proper phase ordering
 * - Integration with raylib's window and rendering system
 */

#include <ecs/component.h>
#include <ecs/system.h>

/**
 * Creates a pre-configured ECS world with camera support.
 *
 * This function creates a complete ECS registry instance that is ready
 * for immediate use. It automatically registers all built-in components
 * and sets up the necessary infrastructure for a typical game application.
 *
 * The world includes support for:
 * - Built-in components (Transform, RigidBody, Collider, Behaviour, etc.)
 * - Built-in systems (physics, rendering, collision, etc.)
 * - Camera integration for rendering
 *
 * @return Pointer to created ECS world, or NULL on failure.
 *
 * @see EcsLoop() for running the main game loop
 * @see EcsFree() for freeing the world when done
 * @see WorldMainCamera() to retrieve the main camera
 * @see EcsRegistry() for creating a basic empty registry
 */
ECS *EcsWorld(void);

/**
 * Runs the main ECS game loop with proper phase ordering.
 *
 * This function implements the complete game loop that processes
 * all registered systems in their correct execution phases. It handles
 * the automatic calling of systems during their proper execution phase.
 *
 * The loop continues until the window is closed.
 *
 * @param world The ECS world to run, created with EcsWorld().
 *
 * @note This function blocks until the window is closed.
 *       For custom game loop control, consider manually calling
 *       the ECS system execution functions.
 *
 * @see EcsWorld() for creating the world
 * @see EcsLayer enum for available execution phases
 */
void EcsLoop(ECS *world);

/**
 * @brief Retrieves the world main camera if exists.
 *
 * @param ecs The ECS world registry.
 * @return Camera2D component pointer or NULL if not found.
 */
Camera *WorldMainCamera(ECS *ecs);

#endif
