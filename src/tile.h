#pragma once

#include <stdint.h>
#include <stdlib.h>

#include "vector.h"

// Must always be 4 because a tile is always divided into two rows and two
// columns in a quadtree.
#define TILE_CHILD_COUNT (4)

typedef struct Tile Tile;

struct Tile {
  /** 0, 1, 2, 3, 4, or 5. */
  uint8_t face;

  /** The level of this tile. */
  uint8_t level;

  /** The position of this tile relative to the upper-left corner, in integer
   * tile coordinates. */
  Vec2 pos;
};

void tileSubdivide(Tile *tile, Tile children[4]);
void tileFilename(char *into, size_t length, Tile *tile, char *prefix,
                  char *suffix);
