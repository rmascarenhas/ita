/* strassen.c - apply Strassen's algorithm to multiply two arbitrary matrices.
 *
 * Strassen's algorithm for matrix multiplication is know for producing a set
 * of carefully disposed sums of quarters of a matrix that can generate the
 * product between two matrices yielding only 7 recursive calls. This makes the
 * algorithm run in Theta(n^lg 7) time, which is asymptotically faster than the
 * intuitive (or naive divide-and-conquer) algorithms, which run on Theta(n^3)
 * time.
 *
 * The input for this program are two square matrices of previously known size.
 * The data for the matrices are read from two files passed on the command line.
 * After applying the multiplication, this program will print the resulting
 * matrix to the standard output and finish.
 *
 * Usage:
 *
 * 	$ ./strassen <size> <m1-data> <m2-data>
 *
 * 	size    - the dimensions of both matrices (which must be square)
 * 	m1-data - a file containing data for the first matrix. Rows are separated
 * 	          by new lines and elements by spaces or tabs.
 * 	m2-data - same as `m1-data`, but for the second matrix.
 *
 * Author: Renato Mascarenhas Costa
 */

#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"

static const char *progname = "strassen";

static void usage();
static void pexit(const char *fname);

static long parse_size(char *size);
static void load_matrix(struct matrix *m, const char *filename, long size);
static int element_printer(double el, long row, long col);

int
main(int argc, char *argv[])
{
	if (argc != 4)
		usage();

	long size = parse_size(argv[1]);

	/* C = A x B */
	struct matrix a, b, c;

	load_matrix(&a, argv[2], size);
	load_matrix(&b, argv[3], size);

	if (matrix_init(&c, size) == -1)
		pexit("matrix_init");

	if (matrix_multiply(&a, &b, &c) == -1)
		pexit("matrix_multiply");

	matrix_traverse(&c, element_printer);
	printf("\n");

	matrix_destroy(&a);
	matrix_destroy(&b);
	matrix_destroy(&c);

	exit(EXIT_SUCCESS);
}

static long
parse_size(char *size)
{
	long s;
	char *endptr;

	s = strtol(size, &endptr, 10);
	if (endptr == size) {
		fprintf(stderr, "%s: %s: not a valid size\n", progname, size);
		exit(EXIT_FAILURE);
	}

	return s;
}

static void
load_matrix(struct matrix *m, const char *filename, long size)
{
	FILE *data;
	data = fopen(filename, "r");
	if (data == NULL)
		pexit("fopen");

	if (matrix_init(m, size) == -1)
		pexit("matrix_init");

	if (matrix_load(m, data) == -1)
		pexit("matrix_load");

	fclose(data);
}

static int
element_printer(double el, long row, __attribute__((unused)) long col)
{
	static long curr_row = 0;

	if (row != curr_row) {
		curr_row = row;
		printf("\n");
	}

	printf("%.2lf ", el);

	return 0;
}

static void
usage()
{
	fprintf(stderr, "Usage: %s <size> <m1-data> <m2-data>\n", progname);
	exit(EXIT_FAILURE);
}

static void
pexit(const char *fname)
{
	perror(fname);
	exit(EXIT_FAILURE);
}
