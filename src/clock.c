#include <sys/time.h>

double getTime(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  long long milliseconds = tv.tv_sec * 1000LL + tv.tv_usec / 1000;
  return milliseconds / 1000.0;
}