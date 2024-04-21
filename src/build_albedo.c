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

/** Samples from the albedo image in options into the provided image. */
static void sampleAlbedo(BuildOptions *options, Tile *tile, Image *image) {
  assert(options != NULL);
  assert(tile != NULL);
  assert(image != NULL);

  Image *source_image = options->albedo_image;

  int sample_count = options->sample_count / (1 << tile->level);

  for (size_t y = 0; y < image->meta.height; y++) {
    for (size_t x = 0; x < image->meta.width; x++) {
      Vec2 tile_pixel = {.x = x, .y = y};
      float result[3] = {0, 0, 0};

      for (int sample = 0; sample < sample_count; sample++) {
        Vec2 tile_fraction = {.x = tile_pixel.x / (image->meta.width - 1),
                              .y = tile_pixel.y / (image->meta.height - 1)};

        Vec2 sample_coords = tileOffsetToEquirectangularImageCoord(
            tile, tile_fraction, vec3CubeRandomOffset(tile, &image->meta),
            &source_image->meta);

        for (int channel = 0; channel < image->meta.channel_count; channel++) {
          result[channel] += sampleChannel8Bilinear(source_image, sample_coords, channel);
        }
      }

      for (int channel = 0; channel < image->meta.channel_count; channel++) {
        writeChannel8(image, tile_pixel, channel,
                      result[channel] / sample_count);
      }
    }
  }
}

#define FILENAME_CAPACITY (256)
void buildAlbedoTile(BuildOptions *options, BuildStats *stats, Tile *tile) {
  assert(options != NULL);
  assert(stats != NULL);
  assert(tile != NULL);

  if (tile->level > options->texture_max_level) {
    return;
  }

  uint32_t size = options->texture_resolution;

  Image out;
  // 1 byte per pixel (8bpp), and three channels.
  imageInitWithOptions(&out, size, size, 1, 3);

  sampleAlbedo(options, tile, &out);

  char filename[FILENAME_CAPACITY];
  tileFilename(filename, FILENAME_CAPACITY, tile, "data/", "-albedo.png");

  imageWritePng(&out, filename);
  imageDeinit(&out);
}
