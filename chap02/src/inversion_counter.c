#include <stdio.h>
#include <stdlib.h>

#include "shared/utils.h"
#include "shared/dataset.h"

char *progname = "inversion_counter";

#ifndef MSORT_MAX_NUMS
#  define MSORT_MAX_NUMS (1000000)
#endif

#define AllocNums(p, size) { \
  p = malloc((size) * sizeof(int)); \
  if (p == NULL) { \
    pexit(progname); \
  } \
}

int
merge(int *array, int const p, int const q, int const r) {
  int size_left, size_right;
  int *left, *right;

  int i, j, k;
  int inversions = 0;

  size_left = q - p + 1;
  size_right = r - q;

  AllocNums(left, size_left + 1);
  AllocNums(right, size_right + 1);

  /* add sentinel */
  int sentinel = INT_MAX;
  left[size_left] = right[size_right] = sentinel;

  /* copy left and right arrays */
  for (i = 1; i <= size_left; ++i) {
    left[i - 1] = array[p + i - 1];
  }

  for (j = 1; j <= size_right; ++j) {
    right[j - 1] = array[q + j];
  }

  i = j = 0;
  for (k = p; k <= r; ++k) {
    if (left[i] <= right[j]) {
      array[k] = left[i];
      ++i;
    } else {
      array[k] = right[j];
      ++j;

      if (left[i] != sentinel) {
        inversions += (size_left - i);
      }
    }
  }

  free(left);
  free(right);

  return inversions;
}

int
count_inversions_rec(int *array, int const start, int const end) {
  if (start >= end) {
    return 0;
  }

  int mid = (start + end)/2,
      inversions = 0;

  inversions += count_inversions_rec(array, start, mid);
  inversions += count_inversions_rec(array, mid + 1, end);
  inversions += merge(array, start, mid, end);

  return inversions;
}

/* Counts the number of inversions on the given `array' using a slightly modified
 * version of the merge sort algorithm */
int
count_inversions(int *array, int const size) {
  return count_inversions_rec(array, 0, size - 1);
}

int
main(int argc, char *argv[]) {
  if (dataset_init(argc, argv) == -1) {
    dataset_usage();
    return EXIT_FAILURE;
  }

  int size, inversions;
  int *buf = malloc(MSORT_MAX_NUMS * sizeof(int));
  if (buf == NULL) {
    pexit(progname);
  }

  size = dataset_get(buf, MSORT_MAX_NUMS);
  if (size == -1) {
    pexit(progname);
  }

  inversions = count_inversions(buf, size);
  printf("%d inversions found.\n", inversions);

  free(buf);

  return EXIT_SUCCESS;
}
