
#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "build.h"
#include "image.h"
#include "mem.h"

Image getAlbedo() {
  Image albedo;

  imageInitFromPnm(&albedo, "/Users/forest/Downloads/lroc_color_poles.ppm");
  imagePrintMetadata(&albedo);

  // Make sure this is equirectangular.
  assert(albedo.meta.width == albedo.meta.height * 2);

  return albedo;
}

Image getHeightmap() {
  Image height;
  imageInitFromPnm(
      &height,
      "/Users/forest/Downloads/Lunar_LRO_LOLA_Global_LDEM_118m_Mar2014.pgm");

  imagePrintMetadata(&height);

  // Make sure this is equirectangular.
  assert(height.meta.width == height.meta.height * 2);

  return height;
}

void buildHeight() {
  Image albedo = getAlbedo();
  Image height = getHeightmap();

  uint32_t texture_resolution = 512;
  uint32_t vertex_resolution = 32;

  build((BuildOptions){
      .texture_max_level =
          ceil(log2(albedo.meta.width / texture_resolution / 4)),
      .vertex_max_level =
          floor(log2(height.meta.width / vertex_resolution / 4)),
      .texture_resolution = texture_resolution,
      .vertex_resolution = vertex_resolution,
      .sample_count = 48,
      .build_mode = BUILD_MODE_ALBEDO | BUILD_MODE_HEIGHT | BUILD_MODE_NORMAL,
      .height_image = &height,
      .albedo_image = &albedo,
  });

  imageDeinit(&albedo);
  imageDeinit(&height);
}

int main(int argc, char *argv[]) {
  buildHeight();
  memLog();
}