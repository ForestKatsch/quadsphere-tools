#include "tile.h"

#include <assert.h>
#include <stdio.h>

void tileSubdivide(Tile *tile, Tile children[4]) {
  assert(tile != NULL);

  for (uint8_t y = 0; y < 2; y++) {
    for (uint8_t x = 0; x < 2; x++) {
      uint8_t i = (y * 2) + x;
      children[i] = (Tile){
          .face = tile->face,
          .level = tile->level + 1,
          .pos = (Vec2){.x = tile->pos.x * 2 + x, .y = tile->pos.y * 2 + y}};
    }
  }
}

char *face_name[6] = {"Z+", "X+", "Y+", "Z-", "X-", "Y-"};

void tileFilename(char *into, size_t length, Tile *tile, char *prefix,
                  char *suffix) {
  snprintf(into, length, "%s%s%d/%d-%d%s", prefix, face_name[tile->face],
           tile->level, (int)tile->pos.x, (int)tile->pos.y, suffix);
}
