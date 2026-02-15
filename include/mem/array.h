#ifndef MEM_ARRAY_H
#define MEM_ARRAY_H

/**
 * @file array.h
 * @brief Dynamic array memory management utilities
 *
 * This header provides low-level memory management functions for dynamic arrays
 * with automatic capacity growth. Used internally by the ECS registry for
 * managing entities, components, and other dynamic collections.
 *
 * All functions use exponential growth strategy to minimize reallocation
 * frequency and provide amortized O(1) push operations.
 *
 * It uses two numbers, `count` and `total`:
 * total is the allocation size, usually 2^n
 * count is the array length, smaller than total
 */

#include <stddef.h>

/**
 * Allocates or reallocates memory for a dynamic array.
 *
 * Acts as a smart allocation wrapper that handles both initial allocation
 * and subsequent reallocations. When ptr is NULL or initial is <= 1,
 * performs a fresh malloc. Otherwise performs realloc.
 *
 * @param ptr   Pointer to existing array (NULL for new allocation)
 * @param total Number of elements to allocate
 * @param count Number of elements in the array, smaller than total
 * @param size  Size of each element in bytes
 * @return Pointer to allocated memory, NULL on failure
 */
void *MemReallocArray(void *ptr, size_t total, size_t count, size_t size);

/**
 * Ensures a dynamic array has sufficient capacity for a given number of
 * elements.
 *
 * Uses exponential growth strategy: when capacity is reached, the array
 * doubles in size. This provides amortized O(1) push operations.
 *
 * @param array Pointer to the array pointer
 * @param total Current capacity of the array
 * @param count Required number of elements
 * @param size  Size of each element in bytes
 * @return New capacity after growth, 0 on allocation failure
 */
size_t MemEnsureCapacity(void **array, size_t total, size_t count, size_t size);

/**
 * Adds an element to the end of a dynamic array.
 *
 * Automatically ensures sufficient capacity before adding the element.
 * Copies the element data into the array using memcpy.
 *
 * @param array   Pointer to the array pointer
 * @param total   Current capacity of the array
 * @param count   Current number of elements in the array
 * @param element Pointer to element data to copy
 * @param size    Size of each element in bytes
 * @return New total capacity after potential growth, 0 on failure
 */
size_t MemPushBack(void **array, size_t total, size_t count, void *element,
                   size_t size);

#endif
