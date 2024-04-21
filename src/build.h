#pragma once
#include <stdatomic.h>
#include <stdint.h>

#include "image.h"
#include "tile.h"

typedef struct BuildOptions BuildOptions;
typedef struct BuildStats BuildStats;

typedef enum BuildMode BuildMode;

enum BuildMode {
  BUILD_MODE_ALBEDO = 1 << 0,
  BUILD_MODE_HEIGHT = 1 << 1,
  BUILD_MODE_NORMAL = 1 << 2,
};

struct BuildOptions {
  /** The number of pixels per square face. */
  uint32_t texture_resolution;

  /** The number of points per square face. */
  uint32_t vertex_resolution;

  /** The deepest subdivision we'll traverse to. All tiles from level 0 to
   * `max_level` (inclusive) will be generated. */
  uint8_t texture_max_level;

  uint8_t vertex_max_level;

  /** Number of source samples per pixel. */
  uint16_t sample_count;

  /** Heightmap (equirectangular) image. May be null. */
  Image *height_image;

  /** Albedo (equirectangular) image. May be null. */
  Image *albedo_image;

  BuildMode build_mode;
};

struct BuildStats {
  _Atomic uint64_t completed_tile_count;
  uint64_t total_tile_count;
};

// build.c
void build(BuildOptions options);
void buildStatsIncrementTile(BuildStats *stats);

// build_height.c
void buildHeightTile(BuildOptions *options, BuildStats *stats, Tile *tile);

// build_albedo.c
void buildAlbedoTile(BuildOptions *options, BuildStats *stats, Tile *tile);
