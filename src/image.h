#pragma once

#include <stdint.h>
#include <stdlib.h>

#define IMAGE_FILENAME_CAPACITY (256)

typedef struct ImageMetadata {
  uint32_t width;
  uint32_t height;

  /** 8bpp = 1 byte per pixel; 16bpp = 2 bytes per pixel */
  uint8_t bytes_per_pixel;
  uint8_t channel_count;

  /** Bytes needed to store this image in memory. */
  size_t byte_count;
  size_t index_count;

  /** Optional filename this file was loaded from or will be written to. */
  char filename[IMAGE_FILENAME_CAPACITY];
} ImageMetadata;

typedef struct Image {
  ImageMetadata meta;
  union {
    void *data;
    uint8_t *data8;
    uint16_t *data16;
  };
} Image;

// image.c
void imageMetadataInit(ImageMetadata *meta);
void imageInit(Image *img);
void imageInitWithOptions(Image *img, uint32_t width, uint32_t height,
                          uint8_t bytes_per_pixel, uint8_t channel_count);
void imageDeinit(Image *img);

void imagePrintMetadata(Image *img);

// image_pnm.c
void imageInitFromPnm(Image *img, char *filename);

void imageWritePng(Image *img, char *filename);
void imageWriteElevation(Image *elevation, Image *normal, char *filename);