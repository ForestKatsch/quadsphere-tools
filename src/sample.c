#include "sample.h"

#include <assert.h>
#include <math.h>

/** Given a floating-point coordinate, returns the index into this image's raw
 * data at which the first channel for this pixel is found. Will round the
 * coordinates to the nearest pixel. */
static size_t coordsToIndex(ImageMetadata *meta, Vec2 pixel, uint8_t channel) {
  assert(meta != NULL);
  assert(channel < meta->channel_count);

  size_t x = floor(pixel.x);
  size_t y = floor(pixel.y);

  assert(x >= 0 && x < meta->width);
  assert(y >= 0 && y < meta->height);

  size_t index = y * (size_t)(meta->width) + x;

  // Multiply by channel count, then add channel, then multiply by
  // bytes-per-pixel.
  index *= meta->channel_count;
  index += channel;

  assert(index < meta->byte_count);

  return index;
}

uint8_t sampleChannel8(Image *image, Vec2 pixel, uint8_t channel) {
  assert(image != NULL);
  assert(image->meta.bytes_per_pixel == 1);

  size_t index = coordsToIndex(&image->meta, pixel, channel);
  return image->data8[index];
}

uint16_t sampleChannel16(Image *image, Vec2 pixel, uint8_t channel) {
  assert(image != NULL);
  assert(image->meta.bytes_per_pixel == 2);

  size_t index = coordsToIndex(&image->meta, pixel, channel);
  return image->data16[index];
}

float sampleChannel8Bilinear(Image *image, Vec2 pixel, uint8_t channel) {
  assert(image != NULL);
  assert(image->meta.bytes_per_pixel == 1);

  Vec2 c00 = {.x = floor(pixel.x) + 0, .y = floor(pixel.y) + 0};
  Vec2 c01 = {.x = floor(pixel.x) + 0, .y = floor(pixel.y) + 1};
  Vec2 c10 = {.x = floor(pixel.x) + 1, .y = floor(pixel.y) + 0};
  Vec2 c11 = {.x = floor(pixel.x) + 1, .y = floor(pixel.y) + 1};

  c10.x = fmod(c10.x, image->meta.width);
  c11.x = fmod(c11.x, image->meta.width);

  c01.y = fmod(c01.y, image->meta.height);
  c11.y = fmod(c11.y, image->meta.height);

  uint8_t v00 = sampleChannel8(image, c00, channel);
  uint8_t v01 = sampleChannel8(image, c01, channel);
  uint8_t v10 = sampleChannel8(image, c10, channel);
  uint8_t v11 = sampleChannel8(image, c11, channel);

  Vec2 frac = {
      .x = pixel.x - floor(pixel.x),
      .y = pixel.y - floor(pixel.y),
  };

  float e0 = (v00 * (1 - frac.y) + (v01 * frac.y));
  float e1 = (v10 * (1 - frac.y) + (v11 * frac.y));

  return (e0 * (1 - frac.x) + (e1 * frac.x));
}

float sampleChannel16Bilinear(Image *image, Vec2 pixel, uint8_t channel) {
  assert(image != NULL);
  assert(image->meta.bytes_per_pixel == 2);

  Vec2 c00 = {.x = floor(pixel.x) + 0, .y = floor(pixel.y) + 0};
  Vec2 c01 = {.x = floor(pixel.x) + 0, .y = floor(pixel.y) + 1};
  Vec2 c10 = {.x = floor(pixel.x) + 1, .y = floor(pixel.y) + 0};
  Vec2 c11 = {.x = floor(pixel.x) + 1, .y = floor(pixel.y) + 1};

  c10.x = fmod(c10.x, image->meta.width);
  c11.x = fmod(c11.x, image->meta.width);

  c01.y = fmod(c01.y, image->meta.height);
  c11.y = fmod(c11.y, image->meta.height);

  uint16_t v00 = sampleChannel16(image, c00, channel);
  uint16_t v01 = sampleChannel16(image, c01, channel);
  uint16_t v10 = sampleChannel16(image, c10, channel);
  uint16_t v11 = sampleChannel16(image, c11, channel);

  Vec2 frac = {
      .x = pixel.x - floor(pixel.x),
      .y = pixel.y - floor(pixel.y),
  };

  float e0 = (v00 * (1 - frac.y) + (v01 * frac.y));
  float e1 = (v10 * (1 - frac.y) + (v11 * frac.y));

  return (e0 * (1 - frac.x) + (e1 * frac.x));
}

void writeChannel8(Image *image, Vec2 pixel, uint8_t channel, uint8_t value) {
  assert(image != NULL);
  assert(image->meta.bytes_per_pixel == 1);

  size_t index = coordsToIndex(&image->meta, pixel, channel);
  image->data8[index] = value;
}
void writeChannel16(Image *image, Vec2 pixel, uint8_t channel, uint16_t value) {
  assert(image != NULL);
  assert(image->meta.bytes_per_pixel == 2);

  size_t index = coordsToIndex(&image->meta, pixel, channel);
  image->data16[index] = value;
}
