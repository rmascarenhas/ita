#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

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

struct tuple {
  int p1, p2;
};

int
binary_search(int *array, int start, int end, int key) {
  int mid;

  while (start <= end) {
    mid = (start + end)/2;

    if (array[mid] < key) {
      start = mid + 1;
    } else if (array[mid] > key) {
      end = mid - 1;
    } else {
      return mid;
    }
  }

  return -1;
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
find_sum_rec(int *array, int start, int end, int sum, struct tuple *result) {
  if (start < end) {
    int mid = (start + end)/2;

    if ((find_sum_rec(array, start, mid, sum, result) != -1) ||
        (find_sum_rec(array, mid + 1, end, sum, result) != -1)) {
      /* sum found in left or right half */
      return 1;
    }

    /* sum not found left or right: look for a split result */
    int j;

    for (j = 0; j <= mid; ++j) {
      result->p1 = j;
      if ((result->p2 = binary_search(array, mid + 1, end, sum - array[j])) != -1) {
        return 1;
      }
    }
  }

  return -1;
}

int
find_sum(int *array, int const size, int sum, struct tuple *result) {
  return find_sum_rec(array, 0, size - 1, sum, result);
}

int
main(int argc, char *argv[]) {
  if (dataset_init(argc, argv) == -1) {
    dataset_usage();
    return EXIT_FAILURE;
  }

  int size;
  long sum = 0;
  char *endptr;
  struct tuple result;

  if (optind >= argc) {
    Fatal("Need to pass the number you are looking for\n");
  }

  sum = strtol(argv[optind], &endptr, 10);
  if (sum == LONG_MIN || sum == LONG_MAX || endptr == argv[optind]) {
    Fatal("Invalid sum given\n");
  }

  int *buf = malloc(MSORT_MAX_NUMS * sizeof(int));
  if (buf == NULL) {
    pexit(progname);
  }

  size = dataset_get(buf, MSORT_MAX_NUMS);
  if (size == -1) {
    pexit(progname);
  }

  merge_sort(buf, size);
  if (find_sum(buf, size, sum, &result) == -1) {
    printf("No two elements with sum %ld\n", sum);
  } else {
    printf("%ld = %d + %d\n", sum, buf[result.p1], buf[result.p2]);
  }

  free(buf);

  return EXIT_SUCCESS;
}
