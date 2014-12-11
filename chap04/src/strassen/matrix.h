/* matrix.h - basic data-structure for representing a matrix in memory.
 * Only square matrices are supported. */

#ifndef CLRS_MATRIX_H
#define CLRS_MATRIX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_LINE_SIZE (1024)
#define ELEMENT_DELIMITERS (" \t")

enum matrix_op { MATRIX_SUM, MATRIX_DIFFERENCE };

struct matrix {
	long size;
	double **data;
};

/* initializes a square matrix of the given `size`, allocating all memory needed
 * in advance. */
int matrix_init(struct matrix *m, long size);

/* allows a previously initialized matrix struct to be initialized with the contents
 * of a given `file`. Each line in the file represent a row in the matrix. Elements
 * in a row a separated by space or tab characters. Passing a file with insufficient
 * or too much data is an error. The state of the passed struct is undefined in that
 * case. */
int matrix_load(struct matrix *m, FILE *file);

/* apply Strassen's algorithm for matrix multiplication. The result of the multiplication
 * of the matrices `a` and `b` are placed in the previously initialized matrix `c`.
 * Passing a result matrix with different size is an error. */
int matrix_multiply(struct matrix *a, struct matrix *b, struct matrix *c);

/* traverses a matrix, calling the specified function for each element. If the
 * function returns a value different than zero, the traversal is halted. */
int matrix_traverse(struct matrix *m, int (*fn)(double el, long row, long col));

/* destroys all data allocated by the given matrix. */
int matrix_destroy(struct matrix *m);

#endif /* CLRS_MATRIX_H */
