
#include "unit.h"

#include <math.h>
#include <stdio.h>

UnitResult formatSeconds(double seconds) {
  UnitResult result;
  size_t offset = 0;
  double hours = 0;
  double minutes = 0;

  if (seconds > 60 * 60) {
    hours = floor(seconds / (60 * 60));

    offset += snprintf(result.string, UNIT_RESULT_CAPACITY - offset, "%dh ",
                       (int)hours);
  }

  seconds -= hours * 60 * 60;

  if (seconds > 60) {
    minutes = floor(seconds / 60);
    offset += snprintf(result.string + offset, UNIT_RESULT_CAPACITY - offset,
                       "%dm ", (int)minutes);
  }

  seconds -= minutes * 60;

  if (hours > 0 || minutes > 0) {
    offset += snprintf(result.string + offset, UNIT_RESULT_CAPACITY - offset,
                       "%02ds", (int)seconds);
  } else {
    offset += snprintf(result.string + offset, UNIT_RESULT_CAPACITY - offset,
                       "%02.2fs", seconds);
  }

  return result;
}

UnitResult formatBytes(size_t bytes) {
  UnitResult result;

  if (bytes < 1024) {
    snprintf(result.string, UNIT_RESULT_CAPACITY, "%zub", bytes);
  } else if (bytes < 1024 * 1024) {
    snprintf(result.string, UNIT_RESULT_CAPACITY, "%.2fkB", bytes / 1024.0);
  } else if (bytes < 1024 * 1024 * 1024) {
    snprintf(result.string, UNIT_RESULT_CAPACITY, "%.2fMB",
             bytes / (1024 * 1024.0));
  } else {
    snprintf(result.string, UNIT_RESULT_CAPACITY, "%.2fGB",
             bytes / (1024 * 1024 * 1024.0));
  }

  return result;
}