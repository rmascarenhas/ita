#include <stdio.h>
#include <stdlib.h>

#include "shared/utils.h"
#include "shared/dataset.h"

char *progname = "insertion_sort";

#ifndef ISORT_MAX_NUMS
#  define ISORT_MAX_NUMS (1000000)
#endif

/* Sorts the given `array' with size `size' using the insertion sort algorithm.
 * It has O(n^2) complexity and is one of the simplest algorithms for sorting.
 * This function is an implementation of the pseudocode provided in the CLRS book
 * and works only with lists of integers.
 */
void
insertion_sort(int *array, int const size) {
  int i, j, key;

  for (j = 1; j < size; ++j) {
    key = array[j];
    i = j - 1;

    /* insert array[j] into the sorted sequence array[1 .. j-1] */
    while (i >= 0 && array[i] > key) {
      array[i + 1] = array[i];
      --i;
    }

    array[i + 1] = key;
  }
}

int
main(int argc, char *argv[]) {
  if (dataset_init(argc, argv) == -1) {
    dataset_usage();
    return EXIT_FAILURE;
  }

  int size;
  int *buf = malloc(ISORT_MAX_NUMS * sizeof(int));
  if (buf == NULL) {
    pexit(progname);
  }

  size = dataset_get(buf, ISORT_MAX_NUMS);
  if (size == -1) {
    pexit(progname);
  }

  insertion_sort(buf, size);
  dataset_print(buf, size);

  free(buf);

  return EXIT_SUCCESS;
}
