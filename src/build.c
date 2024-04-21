#include "build.h"

#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "mem.h"

static void buildStatsInit(BuildStats *stats, BuildOptions *options) {
  assert(stats != NULL);

  stats->completed_tile_count = 0;
  stats->total_tile_count = 0;

  for (int level = 0; level <= options->max_level; level++) {
    stats->total_tile_count += (uint64_t)1 << (level * 2);
  }

  stats->total_tile_count *= 6;
}

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static void buildStatsPrintProgress(BuildStats *stats) {
  assert(stats != NULL);
  assert(stats->total_tile_count > 0);

  float progress_fraction =
      stats->completed_tile_count / (float)stats->total_tile_count;

  pthread_mutex_lock(&lock);
  printf("\33[2K\r");

  printf("[");

  int8_t percent = round(progress_fraction * 100);

  for (int8_t i = 0; i < percent; i++) {
    printf("=");
  }

  for (int8_t i = percent; i < 100; i++) {
    printf(" ");
  }

  printf("] %'llu / %'llu", stats->completed_tile_count,
         stats->total_tile_count);

  fflush(stdout);
  pthread_mutex_unlock(&lock);
}

void buildStatsIncrementTile(BuildStats *stats) {
  assert(stats != NULL);
  assert(stats->total_tile_count > 0);

  atomic_fetch_add(&stats->completed_tile_count, 1);

  /** Print every 0.01% or if completed */
  if (stats->completed_tile_count % (stats->total_tile_count / (100 * 100)) ==
      0) {
    buildStatsPrintProgress(stats);
  }
}

static void buildFace(BuildOptions *options, BuildStats *stats, uint8_t face) {
  Tile tile = {.face = face, .level = 0, .pos = (Vec2){.x = 0, .y = 0}};
  buildHeightTile(options, stats, &tile);
}

typedef struct BuildThreadInfo BuildThreadInfo;

struct BuildThreadInfo {
  BuildOptions *options;
  BuildStats *stats;
  uint8_t face;
};

static void *buildFaceThreadFn(void *data) {
  BuildThreadInfo *info = (BuildThreadInfo *)data;

  buildFace(info->options, info->stats, info->face);
  memFree(data);
  return NULL;
}

static pthread_t buildFaceThread(BuildOptions *options, BuildStats *stats,
                                 uint8_t face) {
  pthread_t t = 0;
  BuildThreadInfo *info = memAllocate(sizeof(BuildThreadInfo));

  *info = (BuildThreadInfo){
      .options = options,
      .stats = stats,
      .face = face,
  };

  int result = pthread_create(&t, NULL, buildFaceThreadFn, info);

  assert(result == 0);

  return t;
}

void build(BuildOptions options) {
  BuildStats stats;
  buildStatsInit(&stats, &options);

  buildStatsPrintProgress(&stats);

  pthread_t threads[6];

  // Six faces on a quadsphere.
  for (int face = 0; face < 6; face++) {
    threads[face] = buildFaceThread(&options, &stats, face);
  }

  for (int i = 0; i < 6; i++) {
    pthread_join(threads[i], NULL);
  }

  buildStatsPrintProgress(&stats);

  // Close out progress
  printf("\n");
}