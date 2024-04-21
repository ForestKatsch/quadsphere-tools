#include "file.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

void mkpath(char *path) {
  char element[PATH_MAX];
  char *p = NULL;
  size_t len;

  snprintf(element, sizeof(element), "%s", path);
  len = strlen(element);
  if (element[len - 1] == '/') element[len - 1] = 0;
  for (p = element + 1; *p; p++)
    if (*p == '/') {
      *p = 0;
      mkdir(element, S_IRWXU);
      *p = '/';
    }
  mkdir(element, S_IRWXU);
}