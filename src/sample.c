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
