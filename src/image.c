#include "image.h"

#include <assert.h>
#include <libgen.h>
#include <stdio.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../extern/stb_image_write.h"
#include "file.h"
#include "mem.h"
#include "unit.h"

void imageMetadataInit(ImageMetadata *meta) {
  assert(meta != NULL);
  meta->width = 0;
  meta->height = 0;

  meta->bytes_per_pixel = 1;
  meta->channel_count = 0;

  meta->byte_count = 0;
  meta->index_count = 0;

  meta->filename[0] = '\0';
}

void deinitImageMetadata(ImageMetadata *meta) { assert(meta != NULL); }

void imageInit(Image *img) {
  assert(img != NULL);
  imageMetadataInit(&img->meta);
  img->data = NULL;
}

void imageInitWithOptions(Image *img, uint32_t width, uint32_t height,
                          uint8_t bytes_per_pixel, uint8_t channel_count) {
  imageInit(img);
  img->meta.width = width;
  img->meta.height = height;
  img->meta.bytes_per_pixel = bytes_per_pixel;
  img->meta.channel_count = channel_count;

  img->meta.index_count = width * height * channel_count;
  img->meta.byte_count = img->meta.index_count * bytes_per_pixel;

  img->data = memAllocate(img->meta.byte_count);
}

void imageDeinit(Image *img) {
  assert(img != NULL);

  deinitImageMetadata(&img->meta);

  if (img->data != NULL) {
    memFree(img->data);
    img->data = NULL;
  }
}

void imagePrintMetadata(Image *img) {
  assert(img != NULL);
  assert(img->meta.byte_count > 0);

  printf("%s: %s (size: %d × %d, bpp: %d)\n",
         img->meta.filename[0] == '\0' ? "(unnamed)" : img->meta.filename,
         formatBytes(img->meta.byte_count).string, img->meta.width,
         img->meta.height, img->meta.bytes_per_pixel * 8);
}

void imageWritePng(Image *img, char *filename) {
  assert(img != NULL);
  assert(img->meta.byte_count > 0);

  char *directory = dirname(filename);
  mkpath(directory);

  stbi_write_png(filename, img->meta.width, img->meta.height,
                 img->meta.channel_count, img->data,
                 img->meta.byte_count / img->meta.height);
}

static void imageWriteJsonElements(Image *img, FILE *f) {
  for (size_t i = 0; i < img->meta.index_count; i++) {
    if (i != 0) {
      fprintf(f, ",");
    }
    if (img->meta.bytes_per_pixel == 2) {
      fprintf(f, "%d", img->data16[i]);
    } else {
      fprintf(f, "%d", img->data8[i]);
    }
  }
}

void imageWriteElevation(Image *elevation, Image *normal, char *filename) {
  assert(elevation != NULL);
  assert(elevation->meta.byte_count > 0);

  char *directory = dirname(filename);
  mkpath(directory);

  FILE *f = fopen(filename, "wb");

  if (f == NULL) {
    fprintf(stderr, "! unable to open '%s' for writing, exiting\n", filename);
    abort();
  }

  fprintf(f, "{\"size\":[%d,%d],\"e\":[", elevation->meta.width,
          elevation->meta.height);

  imageWriteJsonElements(elevation, f);

  if (normal != NULL) {
    assert(elevation->meta.width == normal->meta.width);
    assert(elevation->meta.height == normal->meta.height);

    fprintf(f, "],\"n\":[");
    imageWriteJsonElements(normal, f);
  }

  fprintf(f, "]}");

  fclose(f);
}