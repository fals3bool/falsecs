#include <mem/array.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void *MemReallocArray(void *ptr, size_t total, size_t count, size_t size) {
  if (ptr == NULL || count <= 1)
    return malloc(size * total);
  return realloc(ptr, size * total);
}

size_t MemEnsureCapacity(void **array, size_t total, size_t count,
                         size_t size) {
  if (count >= total) {
    size_t new_total = total ? total * 2 : 4;
    while (new_total < count)
      new_total *= 2;
    void *new_array = MemReallocArray(*array, new_total, count, size);
    if (!new_array)
      return 0;

    *array = new_array;
    return new_total;
  }
  return total;
}

size_t MemPushBack(void **array, size_t total, size_t count, void *element,
                   size_t size) {
  size_t new_total = MemEnsureCapacity(array, total, count + 1, size);
  if (!new_total)
    return 0;
  void *dest = (uint8_t *)(*array) + count * size;
  memcpy(dest, element, size);
  return new_total;
}
