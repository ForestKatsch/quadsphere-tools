
#include "unit.h"

#include <stdio.h>

UnitResult formatBytes(size_t bytes) {
  UnitResult result;

  if (bytes < 1000) {
    snprintf(result.string, UNIT_RESULT_CAPACITY, "%zub", bytes);
  } else if (bytes < 1000000) {
    snprintf(result.string, UNIT_RESULT_CAPACITY, "%.2fkB", bytes / 1000.0);
  } else if (bytes < 1000000000) {
    snprintf(result.string, UNIT_RESULT_CAPACITY, "%.2fMB", bytes / 1000000.0);
  } else {
    snprintf(result.string, UNIT_RESULT_CAPACITY, "%.2fGB",
             bytes / 1000000000.0);
  }

  return result;
}