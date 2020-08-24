#include "tools.h"

double wtime() {
  struct timespec t;
  clock_gettime(CLOCK_REALTIME, &t); 
  return t.tv_sec + (double) t.tv_nsec / 1000000000;
}