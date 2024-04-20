
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "image.h"
#include "mem.h"

static void readHeader(ImageMetadata *meta, FILE *f) {
  assert(meta != NULL);
  assert(f != NULL);

  int max_value;

  char format;

  assert(fscanf(f, "P%c\n", &format) == 1);
  assert(fscanf(f, "%d %d\n", &meta->width, &meta->height) == 2);
  assert(fscanf(f, "%d\n", &max_value) == 1);

  meta->bytes_per_pixel = max_value > 255 ? 2 : 1;

  switch (format) {
    case '5':  // P5 = PGM
      meta->channel_count = 1;
      break;
    case '6':  // P6 = PPM
      meta->channel_count = 3;
      break;
    default:
      fprintf(stderr,
              "! only able to read P5 (PGM binary) and P6 (PPM binary) files; "
              "got format %d\n",
              format);
      exit(1);
  }

  meta->byte_count =
      meta->width * meta->height * meta->bytes_per_pixel * meta->channel_count;
}

void imageInitFromPnm(Image *img, char *filename) {
  assert(img != NULL);
  assert(filename != NULL);

  imageInit(img);
  strncpy(img->meta.filename, filename, IMAGE_FILENAME_CAPACITY);

  FILE *f = fopen(filename, "rb");
  assert(f != NULL);

  readHeader(&img->meta, f);

  img->data = memAllocate(img->meta.byte_count);
  assert(fread(img->data, 1, img->meta.byte_count, f) == img->meta.byte_count);

  fclose(f);
}
