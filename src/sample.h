#pragma once

#include "image.h"
#include "vector.h"

/** Nearest-neighbor sampling. Assumes pixel is already clamped. */
uint8_t sampleChannel8(Image *image, Vec2 pixel, uint8_t channel);
uint16_t sampleChannel16(Image *image, Vec2 pixel, uint8_t channel);
float sampleChannel8Bilinear(Image *image, Vec2 pixel, uint8_t channel);
float sampleChannel16Bilinear(Image *image, Vec2 pixel, uint8_t channel);

void writeChannel8(Image *image, Vec2 pixel, uint8_t channel, uint8_t value);
void writeChannel16(Image *image, Vec2 pixel, uint8_t channel, uint16_t value);