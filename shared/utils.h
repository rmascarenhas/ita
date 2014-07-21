#ifndef UTILS_H
#define UTILS_H

#include <limits.h>
#ifndef LONG_MIN
#  define <linux/limits.h>
#endif

#include <errno.h>

#define Fatal(...) { \
  fprintf(stderr, __VA_ARGS__); \
  exit(EXIT_FAILURE); \
}


/* General purpose functions */

extern void pexit(char const *id);

#endif
