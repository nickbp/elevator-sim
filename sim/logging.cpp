#include "sim/logging.h"

#include <stdarg.h>
#include <stdio.h>

bool sim::verbose_enabled = false;

void sim::debug(const char* format, ...) {
  if (!verbose_enabled) {
    return;
  }

  va_list args;
  va_start(args, format);
  fprintf(stdout, "[D] ");
  vfprintf(stdout, format, args);
  va_end(args);
  fprintf(stdout, "\n");
}
