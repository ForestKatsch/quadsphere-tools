
#include "convert.h"

#include <assert.h>
#include <math.h>

#include "image.h"
#include "tile.h"

void vec3Normalize(Vec3 *vec) {
  float length = sqrt(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);
  vec->x /= length;
  vec->y /= length;
  vec->z /= length;
}

void vec3Scale(Vec3 *vec, float scale) {
  vec->x *= scale;
  vec->y *= scale;
  vec->z *= scale;
}

Vec3 vec3Subtract(Vec3 a, Vec3 b) {
  return (Vec3){
      .x = a.x - b.x,
      .y = a.y - b.y,
      .z = a.z - b.z,
  };
}

Vec3 vec3Cross(Vec3 a, Vec3 b) {
  return (Vec3){.x = a.y * b.z - a.z * b.y,
                .y = a.z * b.x - a.x * b.z,
                .z = a.x * b.y - a.y * b.x};
}

static Vec3 tileOffsetToSphericalRandom(Tile *tile, Vec2 offset,
                                        Vec3 sample_offset_cube) {
  assert(offset.x >= -0.1 && offset.x <= 1.1);
  assert(offset.y >= -0.1 && offset.y <= 1.1);

  int sign = tile->face >= 3 ? -1 : 1;

  Vec3 position = {
      .x = (((offset.x + tile->pos.x) / (float)(1 << tile->level)) * 2 - 1) *
           sign,
      .y = ((offset.y + tile->pos.y) / (float)(1 << tile->level)) * 2 - 1,
      .z = sign};

  Vec3 result = {
      .x = position.data[(0 + tile->face) % 3],
      .y = position.data[(1 + tile->face) % 3],
      .z = position.data[(2 + tile->face) % 3],
  };

  vec3Normalize(&result);

  return result;
}

Vec3 tileOffsetToSpherical(Tile *tile, Vec2 offset) {
  return tileOffsetToSphericalRandom(tile, offset, (Vec3){.data = {0, 0, 0}});
}

/** Given spherical coordinates, returns a latitude from -90 to 90 and a
 * longitude from -180 to 180. */
LatLon sphericalToEquirectangular(Vec3 spherical) {
  LatLon ll = {
      .lon = atan2(spherical.z, spherical.x) / M_PI * 180,
      .lat = asin(spherical.y) / M_PI * 180,
  };

  return ll;
}

/** Returns _floating-point_ image coordinates, in pixels, for the given
 * latitude and longitude. */
Vec2 equirectangularToImageCoord(LatLon ll, ImageMetadata *meta) {
  Vec2 coord = {.x = ((ll.lon + 180) / 360) * meta->width,
                .y = ((ll.lat + 90) / 180) * meta->height};

  coord.x = fmod(coord.x + meta->width, meta->width);
  coord.y = fmax(0, fmin(coord.y, meta->height - 1));

  return coord;
}

Vec2 sphericalToEquirectangularImageCoord(Vec3 spherical,
                                          ImageMetadata *equirectangular_meta) {
  return equirectangularToImageCoord(sphericalToEquirectangular(spherical),
                                     equirectangular_meta);
}

Vec2 tileOffsetToEquirectangularImageCoord(
    Tile *tile, Vec2 tile_fraction, Vec3 sample_offset_cube,
    ImageMetadata *equirectangular_meta) {
  return sphericalToEquirectangularImageCoord(
      tileOffsetToSphericalRandom(tile, tile_fraction, sample_offset_cube),
      equirectangular_meta);
}

float randomBetween(float min, float max) {
  return (rand() / (float)RAND_MAX) * (max - min) + min;
}

float box_radius = 0.5;
Vec2 vec2PixelRandomOffset(Vec2 in) {
  return (Vec2){.x = in.x + randomBetween(-box_radius, box_radius),
                .y = in.y + randomBetween(-box_radius, box_radius)};
}

Vec3 vec3CubeRandomOffset(Tile *tile, ImageMetadata *meta) {
  float radius = (1 / (1 << tile->level)) / meta->width;

  return (Vec3){
      .x = randomBetween(-radius, radius),
      .y = randomBetween(-radius, radius),
      .z = randomBetween(-radius, radius),
  };
}