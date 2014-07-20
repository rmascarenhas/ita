#ifndef UTILS_H
#define UTILS_H

#include <limits.h>
#ifndef LONG_MIN
#  define <linux/limits.h>
#endif

#include <errno.h>

/* General purpose functions */

extern void pexit(char const *id);

#endif
