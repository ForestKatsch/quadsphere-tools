
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "image.h"
#include "mem.h"
#include "progress.h"
#include "unit.h"

// Skips comment and newline if present.
static void skipComment(FILE *f) {
  char next = getc(f);
  if (next != '#') {
    ungetc(next, f);
    return;
  }

  while (next != '\n') {
    next = getc(f);
  }
}

static void readHeader(ImageMetadata *meta, FILE *f) {
  assert(meta != NULL);
  assert(f != NULL);

  int max_value;

  char format;

  int items_scanned;
  items_scanned = fscanf(f, "P%c\n", &format);
  assert(items_scanned == 1);

  skipComment(f);

  items_scanned = fscanf(f, "%d %d\n", &meta->width, &meta->height);
  assert(items_scanned == 2);
  items_scanned = fscanf(f, "%d\n", &max_value);
  assert(items_scanned == 1);

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

  meta->byte_count = (size_t)meta->width * meta->height *
                     meta->bytes_per_pixel * meta->channel_count;
}

#define CHUNK_SIZE (1024 * 1024 * 128)

void imageInitFromPnm(Image *img, char *filename) {
  assert(img != NULL);
  assert(filename != NULL);

  imageInit(img);
  strncpy(img->meta.filename, filename, IMAGE_FILENAME_CAPACITY);

  FILE *f = fopen(filename, "rb");

  if (f == NULL) {
    fprintf(stderr, "! unable to load image file '%s', exiting\n", filename);
    abort();
  }

  readHeader(&img->meta, f);

  img->data = memAllocate(img->meta.byte_count);
  size_t byte_read_count = 0;

  fprintf(stderr, "Loading '%s' (%s)...\n", filename,
          formatBytes(img->meta.byte_count).string);

  Progress p;
  progressStart(&p, 1000);
  p.display_percentage = true;

  // If we need to byte-swap the image after load, we arbitrarily say loading is
  // 80% of the total time and byte swapping is 20% of the total time.
  float read_fraction = img->meta.byte_count == 2 ? 0.8 : 1;

  while (byte_read_count < img->meta.byte_count) {
    byte_read_count +=
        fread(img->data + byte_read_count, 1,
              fmin(CHUNK_SIZE, img->meta.byte_count - byte_read_count), f);

    progressReport(&p, byte_read_count / (float)img->meta.byte_count *
                           read_fraction * 1000);
  }

  uint8_t temp;
  if (img->meta.bytes_per_pixel == 2) {
    for (size_t i = 0; i < img->meta.byte_count; i += 2) {
      temp = img->data8[i + 0];

      img->data8[i + 0] = img->data8[i + 1];
      img->data8[i + 1] = temp;

      if (i % CHUNK_SIZE == 0) {
        progressReport(&p, (read_fraction +
                            (i / img->meta.byte_count * (1 - read_fraction))) *
                               1000);
      }
    }
  }

  progressEnd(&p, ceil(byte_read_count / (float)img->meta.byte_count *
                       read_fraction * 1000));

  assert(byte_read_count == img->meta.byte_count);

  fclose(f);
}
