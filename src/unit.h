#pragma once

#include <stdlib.h>

#define UNIT_RESULT_CAPACITY (32)

typedef struct UnitResult UnitResult;
struct UnitResult {
  char string[UNIT_RESULT_CAPACITY];
};

UnitResult formatSeconds(double seconds);
UnitResult formatBytes(size_t bytes);