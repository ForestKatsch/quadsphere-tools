#pragma once
#include <stdatomic.h>
#include <stdint.h>

#include "tile.h"

typedef struct BuildOptions BuildOptions;
typedef struct BuildStats BuildStats;

typedef enum OutputFormat OutputFormat;

enum OutputFormat {
  /** 8-bit PNG */
  OUTPUT_FORMAT_PNG,
  /** raw values as a JSON array in the format `{"d": [1, 2,
                         3, 4, ...]}` */
  OUTPUT_FORMAT_JSON
};

struct BuildOptions {
  /** The number of points per square face. */
  uint32_t resolution;

  /** The deepest subdivision we'll traverse to. All tiles from level 0 to
   * `max_level` (inclusive) will be generated. */
  uint8_t max_level;

  /** Number of source samples per pixel. */
  uint16_t sample_count;

  OutputFormat output_format;
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
