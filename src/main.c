
#include "image.h"
#include "mem.h"
int main(int argc, char *argv[]) {
  Image image;
  imageInitFromPnm(&image,
                   "/Users/forest/Downloads/LRO_LOLA_DEM_SPole75_30m.pgm");

  imagePrintMetadata(&image);

  imageDeinit(&image);

  // imageInitWithOptions();

  memLog();
}