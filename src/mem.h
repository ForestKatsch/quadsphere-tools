
#pragma once

#include <stdlib.h>

#define LOG_ALLOCATIONS (0)

/** Allocates a number of bytes. Asserts if `malloc` returns `NULL`. */
#define memAllocate(bytes) _memAllocate(bytes, __FILE__, __LINE__)

/** Frees the provided pointer. */
#define memFree(pointer) _memFree(pointer, __FILE__, __LINE__)

typedef struct Allocation Allocation;

struct Allocation {
  /** Where the allocation was performed. If NULL, this allocation slot is
   * empty. We don't own this memory and should not free it. */
  char *filename;
  size_t line;

  size_t bytes;

  Allocation *prev;
  Allocation *next;
};

void initAllocation(Allocation *allocation);
void *_memAllocate(size_t bytes, char *filename, size_t line);
void _memFree(void *pointer, char *filename, size_t line);

void memLog(void);
