#include <stdio.h>
#include <stdlib.h>

#include "shared/utils.h"
#include "shared/dataset.h"

char *progname = "merge_sort";

#ifndef MSORT_MAX_NUMS
#  define MSORT_MAX_NUMS (1000000)
#endif

#define AllocNums(p, size) { \
  p = malloc((size) * sizeof(int)); \
  if (p == NULL) { \
    pexit(progname); \
  } \
}

void
merge(int *array, int const p, int const q, int const r) {
  int size_left, size_right;
  int *left, *right;

  int i, j, k;

  size_left = q - p + 1;
  size_right = r - q;

  AllocNums(left, size_left + 1);
  AllocNums(right, size_right + 1);

  /* add sentinel */
  left[size_left] = right[size_right] = INT_MAX;

  /* copy left and right arrays */
  for (i = 1; i <= size_left; ++i) {
    left[i - 1] = array[p + i - 1];
  }

  for (j = 1; j <= size_right; ++j) {
    right[j - 1] = array[q + j];
  }

  i = j = 0;
  for (k = p; k <= r; ++k) {
    array[k] = left[i] <= right[j] ? left[i++] : right[j++];
  }

  free(left);
  free(right);
}

void
merge_sort_rec(int *array, int const start, int const end) {
  if (start < end) {
    int mid = (start + end)/2;

    merge_sort_rec(array, start, mid);
    merge_sort_rec(array, mid + 1, end);
    merge(array, start, mid, end);
  }
}

/* Sorts the given `array' with size `size' using the insertion sort algorithm. */
void
merge_sort(int *array, int const size) {
  merge_sort_rec(array, 0, size - 1);
}

int
main(int argc, char *argv[]) {
  if (dataset_init(argc, argv) == -1) {
    dataset_usage();
    return EXIT_FAILURE;
  }

  int size;
  int *buf = malloc(MSORT_MAX_NUMS * sizeof(int));
  if (buf == NULL) {
    pexit(progname);
  }

  size = dataset_get(buf, MSORT_MAX_NUMS);
  if (size == -1) {
    pexit(progname);
  }

  merge_sort(buf, size);
  dataset_print(buf, size);

  free(buf);

  return EXIT_SUCCESS;
}
