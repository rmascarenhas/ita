#include <stdio.h>
#include <stdlib.h>

#include "shared/utils.h"
#include "shared/dataset.h"

char *progname = "bubble_sort";

#ifndef BSORT_MAX_NUMS
#  define BSORT_MAX_NUMS (1000000)
#endif

/* Sorts the given `array' with size `size' using the bubble sort algorithm. */
void
bubble_sort(int *array, int const size) {
  int i, j, key;

  for (i = 0; i < size - 1; ++i) {
    for (j = size - 1; j > i; --j) {
      if (array[j] < array[j - 1]) {
        key = array[j];
        array[j] = array[j - 1];
        array[j - 1] = key;
      }
    }
  }
}

int
main(int argc, char *argv[]) {
  if (dataset_init(argc, argv) == -1) {
    dataset_usage();
    return EXIT_FAILURE;
  }

  int size;
  int *buf = malloc(BSORT_MAX_NUMS * sizeof(int));
  if (buf == NULL) {
    pexit(progname);
  }

  size = dataset_get(buf, BSORT_MAX_NUMS);
  if (size == -1) {
    pexit(progname);
  }

  bubble_sort(buf, size);
  dataset_print(buf, size);

  free(buf);

  return EXIT_SUCCESS;
}
