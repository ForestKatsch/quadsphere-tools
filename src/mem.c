#include "mem.h"

#include <assert.h>
#include <pthread.h>
#include <stdio.h>

#include "unit.h"

static Allocation root = {
    .filename = NULL,
    .line = 0,
    .bytes = 0,
    .prev = &root,
    .next = &root,
};

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void initAllocation(Allocation *allocation) {
  assert(allocation != NULL);
  allocation->filename = NULL;
  allocation->line = 0;
  allocation->bytes = 0;

  allocation->next = allocation;
  allocation->prev = allocation;
}

static void unlink(Allocation *a) {
  assert(a != NULL);
  Allocation *next = a->next;

  a->prev->next = a->next;
  next->prev = a->prev;
}

static void insert(Allocation *list, Allocation *a) {
  assert(list != NULL);
  assert(a != NULL);

  unlink(a);

  /**
   * BEFORE:
   * list->prev  list     list->next
   *
   * AFTER:
   * list->prev  list  a  list->next
   */

  list->next->prev = a;

  a->prev = list;
  a->next = list->next;

  list->next = a;
}

void *_memAllocate(size_t bytes, char *filename, size_t line) {
  size_t total_size = sizeof(Allocation) + bytes;

  void *pointer = malloc(total_size);

  assert(pointer != NULL);

  Allocation *new = (Allocation *)pointer;

  initAllocation(new);
  new->filename = filename;
  new->line = line;
  new->bytes = bytes;

  pthread_mutex_lock(&lock);
  insert(&root, new);
  pthread_mutex_unlock(&lock);

#if LOG_ALLOCATIONS
  fprintf(stderr, "+ %s from %s L%zu\n", formatBytes(bytes).string, filename,
          line);
#endif

  return (uint8_t *)pointer + sizeof(Allocation);
}

void _memFree(void *pointer, char *filename, size_t line) {
  void *real_pointer = ((uint8_t *)pointer - sizeof(Allocation));
  Allocation *a = (Allocation *)((uint8_t *)pointer - sizeof(Allocation));
  pthread_mutex_lock(&lock);
  unlink(a);
  pthread_mutex_unlock(&lock);

#if LOG_ALLOCATIONS
  fprintf(stderr, "- %s from %s L%zu\n", formatBytes(a->bytes).string,
          a->filename, a->line);
#endif

  free(real_pointer);
}

void memLog(void) {
  Allocation *start = &root;
  Allocation *next = start;

  int unfree_count = 0;

  do {
    if (next == &root) {
      next = next->next;
      continue;
    }

    if (unfree_count == 0) {
      fprintf(stderr, "! memory leaked\n");
    }

    unfree_count += 1;

    size_t bytes = next->bytes;

    fprintf(stderr, "%s from %s L%zu\n", formatBytes(next->bytes).string,
            next->filename, next->line);

    next = next->next;
  } while (next != start);
}