
#pragma once

#include "image.h"
#include "tile.h"
#include "vector.h"

void vec3Normalize(Vec3 *vec);
void vec3Scale(Vec3 *vec, float scale);
Vec3 vec3Subtract(Vec3 a, Vec3 b);
Vec3 vec3Cross(Vec3 a, Vec3 b);

/** Returns a spherical vector direction for a
 * given tile and a fractional (0-to-1) offset
 * within that tile's coordinate space. */
Vec3 tileOffsetToSpherical(Tile *tile, Vec2 offset);

/** Given spherical coordinates,
 * returns a latitude from -90 to 90
 * and a longitude from -180 to 180. */
LatLon sphericalToEquirectangular(Vec3 spherical);

/** Given an equirectangular coordinate, returns the fractional X/Y offset of
 * that pixel within size. */
Vec2 equirectangularToImageCoord(LatLon ll, ImageMetadata *meta);

Vec2 sphericalToEquirectangularImageCoord(Vec3 spherical, ImageMetadata *meta);

Vec2 tileOffsetToEquirectangularImageCoord(Tile *tile, Vec2 tile_fraction,
                                           Vec3 sample_offset_cube,
                                           ImageMetadata *equirectangular_meta);

float randomBetween(float min, float max);
Vec2 vec2PixelRandomOffset(Vec2 in);
Vec3 vec3CubeRandomOffset(Tile *tile, ImageMetadata *meta);