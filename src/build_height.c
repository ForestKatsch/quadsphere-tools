#include <assert.h>
#include <stdlib.h>

#include "build.h"
#include "image.h"
#include "tile.h"

static void buildTile(BuildOptions *options, BuildStats *stats, Tile *tile) {
  assert(options != NULL);
  assert(stats != NULL);
  assert(tile != NULL);

  // Border would go here if desired.
  uint32_t size = options->resolution;

  Image out;
  imageInitWithOptions(&out, size, size, 2, 1);

  imageDeinit(&out);
}

void buildHeightTile(BuildOptions *options, BuildStats *stats, Tile *tile) {
  assert(options != NULL);
  assert(stats != NULL);
  assert(tile != NULL);

  buildTile(options, stats, tile);
  buildStatsIncrementTile(stats);

  if (tile->level < options->max_level) {
    Tile subtiles[TILE_CHILD_COUNT];
    tileSubdivide(tile, subtiles);

    for (int i = 0; i < TILE_CHILD_COUNT; i++) {
      buildHeightTile(options, stats, &subtiles[i]);
    }
  }
}
