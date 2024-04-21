#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "build.h"
#include "convert.h"
#include "image.h"
#include "sample.h"
#include "tile.h"
#include "vector.h"

static void sampleHeight(BuildOptions *options, Tile *tile, Image *image) {
  assert(options != NULL);
  assert(tile != NULL);
  assert(image != NULL);
  assert(image->meta.bytes_per_pixel == 2);

  Image *source_image = options->height_image;

  int sample_count = fmax(options->sample_count / (1 << tile->level), 1);

  for (size_t y = 0; y < image->meta.height; y++) {
    for (size_t x = 0; x < image->meta.width; x++) {
      Vec2 tile_pixel = {.x = x, .y = y};
      float result = 0;

      Vec2 tile_fraction = {.x = tile_pixel.x / (image->meta.width - 1),
                            .y = tile_pixel.y / (image->meta.height - 1)};

      for (int sample = 0; sample < sample_count; sample++) {
        Vec2 sample_coords = tileOffsetToEquirectangularImageCoord(
            tile, tile_fraction, vec3CubeRandomOffset(tile, &image->meta),
            &source_image->meta);

        result += sampleChannel16(source_image, sample_coords, 0);
      }

      writeChannel16(image, tile_pixel, 0, result / sample_count / 2);
    }
  }
}

static void sampleNormal(BuildOptions *options, Tile *tile, Image *image) {
  assert(options != NULL);
  assert(tile != NULL);
  assert(image != NULL);
  assert(image->meta.bytes_per_pixel == 1);

  Image *source_image = options->height_image;

  int sample_count = options->sample_count / (1 << tile->level);

  for (size_t y = 0; y < image->meta.height; y++) {
    for (size_t x = 0; x < image->meta.width; x++) {
      Vec2 tile_pixel = {.x = x, .y = y};
      // center, down, right
      float result[3] = {0, 0, 0};

      for (int sample = 0; sample < sample_count; sample++) {
        Vec3 sample_offset = vec3CubeRandomOffset(tile, &image->meta);

        for (int dir = 0; dir < 3; dir++) {
          Vec2 dir_pixel = {.x = tile_pixel.x, .y = tile_pixel.y};

          if (dir == 1) {
            dir_pixel.y += 1;
          } else if (dir == 2) {
            dir_pixel.x += 1;
          }

          Vec2 tile_fraction = {.x = dir_pixel.x / (image->meta.width - 1),
                                .y = dir_pixel.y / (image->meta.height - 1)};

          Vec2 sample_coords = tileOffsetToEquirectangularImageCoord(
              tile, tile_fraction, sample_offset, &source_image->meta);

          result[dir] +=
              sampleChannel16Bilinear(source_image, sample_coords, 0);
        }
      }

      for (int i = 0; i < 3; i++) {
        result[i] /= sample_count;
      }

      float scale = 0.5 / 1737000;

      Vec3 center = tileOffsetToSpherical(
          tile, (Vec2){.x = tile_pixel.x / image->meta.width,
                       .y = tile_pixel.y / image->meta.height});
      vec3Scale(&center, result[0] * scale + 1);
      Vec3 down = tileOffsetToSpherical(
          tile, (Vec2){.x = tile_pixel.x / image->meta.width,
                       .y = (tile_pixel.y + 1) / image->meta.height});
      vec3Scale(&down, result[1] * scale + 1);
      Vec3 right = tileOffsetToSpherical(
          tile, (Vec2){.x = (tile_pixel.x + 1) / image->meta.width,
                       .y = tile_pixel.y / image->meta.height});
      vec3Scale(&right, result[2] * scale + 1);

      Vec3 normal =
          vec3Cross(vec3Subtract(right, center), vec3Subtract(down, center));

      vec3Normalize(&normal);

      for (int channel = 0; channel < image->meta.channel_count; channel++) {
        writeChannel8(image, tile_pixel, channel,
                      normal.data[channel] * 127 + 128);
      }
    }
  }
}

#define FILENAME_CAPACITY (256)
void buildHeightTile(BuildOptions *options, BuildStats *stats, Tile *tile) {
  assert(options != NULL);
  assert(stats != NULL);
  assert(tile != NULL);

  if (options->build_mode & BUILD_MODE_HEIGHT) {
    if (tile->level <= options->vertex_max_level) {
      Image vertex_height;
      imageInitWithOptions(&vertex_height, options->vertex_resolution,
                           options->vertex_resolution, 2, 1);

      sampleHeight(options, tile, &vertex_height);

      char filename[FILENAME_CAPACITY];
      tileFilename(filename, FILENAME_CAPACITY, tile, "data/", ".json");

      imageWriteElevation(&vertex_height, NULL, filename);
      imageDeinit(&vertex_height);
    }
  }

  if (options->build_mode & BUILD_MODE_NORMAL) {
    if (tile->level <= options->texture_max_level) {
      Image texture_normal;
      imageInitWithOptions(&texture_normal, options->texture_resolution,
                           options->texture_resolution, 1, 3);

      sampleNormal(options, tile, &texture_normal);

      char filename[FILENAME_CAPACITY];
      tileFilename(filename, FILENAME_CAPACITY, tile, "data/", "-normal.png");

      imageWritePng(&texture_normal, filename);
      imageDeinit(&texture_normal);
    }
  }
}
