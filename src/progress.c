#include "progress.h"

#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>

#include "clock.h"
#include "unit.h"
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void progressStart(Progress *p, size_t total) {
  p->display_percentage = false;
  p->display_estimated_time = true;
  p->total = total;
  p->start = getTime();

  p->time_last_printed = getTime();
  p->remaining = 1;

  progressReport(p, 0);
}

void progressReport(Progress *p, size_t count) {
  float progress_fraction = count / (float)p->total;

  int8_t percent = round(progress_fraction * 100);

  pthread_mutex_lock(&lock);
  fprintf(stderr, "\33[2K\r");

  fprintf(stderr, "[");

  for (int8_t i = 0; i < percent; i++) {
    fprintf(stderr, "=");
  }

  for (int8_t i = percent; i < 100; i++) {
    fprintf(stderr, " ");
  }

  fprintf(stderr, "] ");

  if (p->total > 2000 || p->display_percentage == true) {
    fprintf(stderr, "%03.2f%%", progress_fraction * 100);
  } else {
    fprintf(stderr, "%zu / %zu", count, p->total);
  }

  double now = getTime();
  double elapsed = now - p->start;

  if (p->display_estimated_time && elapsed > 5 &&
      (percent > 5 || count > 100)) {
    if ((now - p->time_last_printed) > 1) {
      p->remaining = (elapsed / progress_fraction) - elapsed;
      p->time_last_printed = now;
    }

    fprintf(stderr, " (about %s remaining)",
            formatSeconds(p->remaining).string);
  }

  fflush(stderr);
  pthread_mutex_unlock(&lock);
}

void progressEnd(Progress *p, size_t count) {
  progressReport(p, count);

  pthread_mutex_lock(&lock);
  fprintf(stderr, "\n");
  pthread_mutex_unlock(&lock);
}
