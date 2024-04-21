#pragma once

#include "image.h"
#include "types.h"

/** Nearest-neighbor sampling. */
uint8_t sampleChannel8(Image *image, Vec2 coordinate, uint8_t channel);
uint16_t sampleChannel16(Image *image, Vec2 coordinate, uint8_t channel);