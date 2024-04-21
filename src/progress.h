#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

typedef struct Progress Progress;

struct Progress {
  size_t total;
  bool display_percentage;
  bool display_estimated_time;
  double start;
  double time_last_printed;
  double remaining;
};

void progressStart(Progress *p, size_t total);
void progressReport(Progress *p, size_t count);
void progressEnd(Progress *p, size_t count);