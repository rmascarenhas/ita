/* maximum_subarray.c - implements algorithms for solving the maximum-subarray problem.
 *
 * The maximum-subarray problem involves finding a continguous array within a one-dimensional
 * array whose sum of elements is maximum. It was proposed by Ulf Grenader in 1977 and
 * a linear algorithm (implemented in this program) was proposed by Jay Kadane in 1984.
 *
 * This program also implements a naive and a divide-and-conquer solutions.
 *
 * Author: Renato Mascarenhas Costa
 */

#include <limits.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shared/utils.h"
#include "shared/dataset.h"

char *progname = "maximum_subarray";

#ifndef MSA_MAX_NUMS
#  define MSA_MAX_NUMS (1000000)
#endif

struct maximum_subarray_solution {
	int left;
	int right;
	int sum;
};

/* the naive solution to the maximum-subarray problem tries every possible
 * combination (i, j), i < j and calculates the sum of the subarray a[i .. j],
 * keeping track of the largest one found. When the process is over, it returns
 * the maximum sum.
 *
 * The naive implementation runs in Theta(n^2) time. */
static void
naive_maximum_subarray(int *a, int size, struct maximum_subarray_solution *solution) {
	int sum, max_sum, i, j, max_left, max_right;
	max_sum = INT_MIN;
	max_left = max_right = 0;

	for (i = 0; i < size; ++i) {
		sum = 0;
		for (j = i; j < size; ++j) {
			sum += a[j];
			if (sum > max_sum) {
				max_sum = sum;
				max_left = i;
				max_right = j;
			}
		}
	}

	solution->left = max_left;
	solution->right = max_right;
	solution->sum = max_sum;
}


static void
max_crossing_solution(int *a, int low, int mid, int high, struct maximum_subarray_solution *solution) {
	int left_sum, right_sum, sum, max_left, max_right, i;
	left_sum = right_sum = INT_MIN;
	max_left = max_right = sum = 0;

	for (i = mid; i >= low; --i) {
		sum += a[i];
		if (sum > left_sum) {
			left_sum = sum;
			max_left = i;
		}
	}

	sum = 0;
	for (i = mid + 1; i <= high; ++i) {
		sum += a[i];
		if (sum > right_sum) {
			right_sum = sum;
			max_right = i;
		}
	}

	solution->left = max_left;
	solution->right = max_right;
	solution->sum = left_sum + right_sum;
}

static void
divide_and_conquer_maximum_subarray_rec(int *a, struct maximum_subarray_solution *solution, int low, int high) {
	if (low == high) {
		solution->left = low;
		solution->right = high;
		solution->sum = a[low];
		return;
	}

	struct maximum_subarray_solution left_solution,
					 right_solution,
					 crossing_solution,
					 *best_solution;
	int mid = (low + high) / 2;

	divide_and_conquer_maximum_subarray_rec(a, &left_solution, low, mid);
	divide_and_conquer_maximum_subarray_rec(a, &right_solution, mid + 1, high);
	max_crossing_solution(a, low, mid, high, &crossing_solution);

	if (left_solution.sum > right_solution.sum && left_solution.sum > right_solution.sum) {
		best_solution = &left_solution;
	} else if (right_solution.sum > left_solution.sum && right_solution.sum > crossing_solution.sum) {
		best_solution = &right_solution;
	} else {
		best_solution = &crossing_solution;
	}

	memcpy(solution, best_solution, sizeof(struct maximum_subarray_solution));
}

/* the divide-and-conquer solution to the maximum-subarray problem takes the usual approach
 * of solving the problem in the left and right parts of an array. However, in the combination
 * part, the algorithm has to check whether there is a maximum-subarray that crosses the mid
 * point whose sum is bigger than the solutions for left and right. That can be done in linear
 * time.
 *
 * The divide-and-conquer approach takes Theta(n log(n)) time. */
static void
divide_and_conquer_maximum_subarray(int *a, int size, struct maximum_subarray_solution *solution) {
	divide_and_conquer_maximum_subarray_rec(a, solution, 0, size - 1);
}

/* Kadane's linear algorithm. The idea is that if the maximum subarray of A[1 .. j] is known
 * then we can determine the maximum subarray of A[1 .. j + 1] in constant time. This
 * is achieved by keeping a candidate for the maximum subarray at every step, and
 * swapping when necessary.
 *
 * The algorithm runs in Theta(n) time. */
static void
linear_maximum_subarray(int *a, int size, struct maximum_subarray_solution *solution) {
	if (size <= 0) {
		solution->left = -1;
		solution->right = -1;
		solution->sum = 0;
		return;
	}

	if (size == 1) {
		solution->left = 0;
		solution->right = 0;
		solution->sum = a[0];
		return;
	}

	int max_left, max_right, max_sum, candidate_max, candidate_start, i;

	candidate_start = 0;
	candidate_max = max_sum = a[0];
	max_left = max_right = 0;

	for (i = 1; i < size; ++i) {
		if (candidate_max > 0) {
			candidate_max += a[i];
		} else {
			candidate_max = a[i];
			candidate_start = i;
		}

		if (candidate_max > max_sum) {
			max_sum = candidate_max;
			max_left = candidate_start;
			max_right = i;
		}
	}

	solution->left = max_left;
	solution->right = max_right;
	solution->sum = max_sum;
}

int
main(int argc, char *argv[]) {
  if (dataset_init(argc, argv) == -1) {
	  dataset_usage();
	  exit(EXIT_FAILURE);
  }

  struct maximum_subarray_solution naive_solution;
  struct maximum_subarray_solution divide_and_conquer_solution;
  struct maximum_subarray_solution linear_solution;

  int size;
  int *buf = malloc(MSA_MAX_NUMS * sizeof(int));
  if (!buf) {
	  pexit("malloc");
  }

  size = dataset_get(buf, MSA_MAX_NUMS);
  if (size == -1) {
	  pexit("dataset_get");
  }

  naive_maximum_subarray(buf, size, &naive_solution);
  divide_and_conquer_maximum_subarray(buf, size, &divide_and_conquer_solution);
  linear_maximum_subarray(buf, size, &linear_solution);

  printf("Naive solution: from index %d to %d, sum %d\n",
		  naive_solution.left, naive_solution.right, naive_solution.sum);

  printf("Divide and conquer solution: from index %d to %d, sum %d\n",
		  divide_and_conquer_solution.left, divide_and_conquer_solution.right, divide_and_conquer_solution.sum);

  printf("Linear solution: from index %d to %d, sum %d\n",
		  linear_solution.left, linear_solution.right, linear_solution.sum);

  free(buf);

  exit(EXIT_SUCCESS);
}
