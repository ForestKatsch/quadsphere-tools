
#include <assert.h>
#include <stdio.h>

#include "build.h"
#include "image.h"
#include "mem.h"

Image getHeightmap() {
  Image heightmap;
  imageInitFromPnm(
      &heightmap,
      "/Users/forest/Downloads/Lunar_LRO_LOLA_Global_LDEM_118m_Mar2014.pgm");

  // printf("Heightmap (equirectangular): ");
  // imagePrintMetadata(&heightmap);

  assert(heightmap.meta.width == heightmap.meta.height * 2);

  return heightmap;
}

void buildHeight() {
  Image heightmap = getHeightmap();

  build((BuildOptions){.max_level = 10,
                       .resolution = 256,
                       .sample_count = 8,
                       .output_format = OUTPUT_FORMAT_PNG});

  imageDeinit(&heightmap);
}

int main(int argc, char *argv[]) {
  buildHeight();
  memLog();
}