
#include <stdio.h>
#include <stdlib.h>

#include "shared/utils.h"
#include "shared/dataset.h"

char *progname = "selection_sort";

#ifndef SSORT_MAX_NUMS
#  define SSORT_MAX_NUMS (1000000)
#endif

/* Sorts the given `array' with size `size' using the selection sort algorithm. */
void
selection_sort(int *array, int const size) {
  int i, j, key, min;

  for (j = 0; j < size - 1; ++j) {
    min = j;

    for (i = j; i < size; ++i) {
      if (array[i] < array[min]) {
        min = i;
      }
    }

    key = array[j];
    array[j] = array[min];
    array[min] = key;
  }
}

int
main(int argc, char *argv[]) {
  if (dataset_init(argc, argv) == -1) {
    dataset_usage();
    return EXIT_FAILURE;
  }

  int size;
  int *buf = malloc(SSORT_MAX_NUMS * sizeof(int));
  if (buf == NULL) {
    pexit(progname);
  }

  size = dataset_get(buf, SSORT_MAX_NUMS);
  if (size == -1) {
    pexit(progname);
  }

  selection_sort(buf, size);
  dataset_print(buf, size);

  free(buf);

  return EXIT_SUCCESS;
}
