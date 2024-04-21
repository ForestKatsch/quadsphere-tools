#include "build.h"

#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "mem.h"
#include "progress.h"

Progress p;

static void buildStatsInit(BuildStats *stats, BuildOptions *options) {
  assert(stats != NULL);

  stats->completed_tile_count = 0;
  stats->total_tile_count = 0;

  for (int level = 0;
       level <= fmax(options->texture_max_level, options->vertex_max_level);
       level++) {
    stats->total_tile_count += (uint64_t)1 << (level * 2);
  }

  stats->total_tile_count *= 6;
}

static void buildStatsPrintProgress(BuildStats *stats) {
  assert(stats != NULL);
  assert(stats->total_tile_count > 0);

  progressReport(&p, stats->completed_tile_count);
}

void buildStatsIncrementTile(BuildStats *stats) {
  assert(stats != NULL);
  assert(stats->total_tile_count > 0);

  atomic_fetch_add(&stats->completed_tile_count, 1);

  if (stats->completed_tile_count % 2 == 0) {
    buildStatsPrintProgress(stats);
  }
}

static void buildTile(BuildOptions *options, BuildStats *stats, Tile *tile) {
  if (options->build_mode & BUILD_MODE_ALBEDO) {
    buildAlbedoTile(options, stats, tile);
  }

  if (options->build_mode & (BUILD_MODE_HEIGHT | BUILD_MODE_NORMAL)) {
    buildHeightTile(options, stats, tile);
  }

  buildStatsIncrementTile(stats);

  if (tile->level <
      fmax(options->texture_max_level, options->vertex_max_level)) {
    Tile subtiles[TILE_CHILD_COUNT];
    tileSubdivide(tile, subtiles);

    for (int i = 0; i < TILE_CHILD_COUNT; i++) {
      buildTile(options, stats, &subtiles[i]);
    }
  }
}

static void buildFace(BuildOptions *options, BuildStats *stats, uint8_t face) {
  Tile tile = {.face = face, .level = 0, .pos = (Vec2){.x = 0, .y = 0}};

  buildTile(options, stats, &tile);
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

  fprintf(stderr,
          "Building %llu tiles (texture to %d, vertex to %d, sample_count = "
          "%d)...\n",
          stats.total_tile_count, options.texture_max_level,
          options.vertex_max_level, options.sample_count);
  progressStart(&p, stats.total_tile_count);

  pthread_t threads[6];

  // Six faces on a quadsphere.
  for (int face = 0; face < 6; face++) {
    threads[face] = buildFaceThread(&options, &stats, face);
  }

  for (int i = 0; i < 6; i++) {
    pthread_join(threads[i], NULL);
  }

  progressEnd(&p, stats.completed_tile_count);
}