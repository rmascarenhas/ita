#include "matrix.h"

#define ErrorCase(condition, err, retval) { \
	if (condition) { \
		errno = err; \
		return retval; \
	} \
}

int
matrix_init(struct matrix *m, long size)
{
	ErrorCase(size <= 0, EINVAL, -1);

	long i;

	m->data = malloc(size * sizeof(double *));
	ErrorCase(m->data == NULL, errno, -1);

	for (i = 0; i < size; ++i) {
		m->data[i] = malloc(size * sizeof(double));
		ErrorCase(m->data[i] == NULL, errno, -1);
	}

	m->size = size;
	return 0;
}

static int
insert_at(struct matrix *m, int row, int col, int el)
{
	if (row >= m->size || col >= m->size)
		return -1;

	m->data[row][col] = el;
	return 0;
}

int matrix_load(struct matrix *m, FILE *file)
{
	char line[MAX_LINE_SIZE], *p, *endptr;
	int lines_read = 0, columns_read;
	double el;

	while (fgets(line, MAX_LINE_SIZE, file) != NULL) {
		columns_read = 0;

		p = strtok(line, ELEMENT_DELIMITERS);
		while (p != NULL) {
			el = strtod(p, &endptr);
			ErrorCase(endptr == p, EINVAL, -1);
			ErrorCase(insert_at(m, lines_read, columns_read, el) == -1, ENOMEM, -1);

			++columns_read;
			p = strtok(NULL, ELEMENT_DELIMITERS);
		}

		++lines_read;
	}

	return 0;
}

/* C = A + B */
static void
matrix_operation(struct matrix *a, struct matrix *b, struct matrix *c, int operation) {
	long i, j;

	for (i = 0; i < a->size; ++i) {
		for (j = 0; j < a->size; ++j) {
			if (operation == MATRIX_SUM)
				c->data[i][j] = a->data[i][j] + b->data[i][j];
			else
				c->data[i][j] = a->data[i][j] - b->data[i][j];
		}
	}
}

static void
partition(struct matrix *m, long len, long line, struct matrix *m11, struct matrix *m12, struct matrix *m21, struct matrix *m22)
{
	m11->data[line] = m->data[line];
	m12->data[line] = &(m->data[line][len]);
	m21->data[line] = m->data[line + len];
	m22->data[line] = &(m->data[line + len][len]);
}

int
matrix_multiply(struct matrix *a, struct matrix *b, struct matrix *c)
{
#define InitMatrix(m, size) do { \
	if (matrix_init(&m, size) == -1) \
		return -1; \
} while (0);

#define InitQuadrant(q, size) do { \
	q.data = malloc(size * sizeof(double *)); \
	if (q.data == NULL) \
		return -1; \
} while (0);

	ErrorCase(a == NULL || b == NULL || c == NULL, EINVAL, -1);
	ErrorCase(!(a->size == b->size && b->size == c->size), EINVAL, -1);

	/* recursion base case */
	if (a->size == 1) {
		c->data[0][0] = a->data[0][0] * b->data[0][0];
		return 0;
	}

	long i, len = a->size/2;

	/* artificially create each of the quadrants of the input matrices */
	struct matrix a11, a12, a21, a22,
		      b11, b12, b21, b22,
		      c11, c12, c21, c22;

	a11.size = a12.size = a21.size = a22.size =
		b11.size = b12.size = b21.size = b22.size =
		c11.size = c12.size = c21.size = c22.size = len;

	InitQuadrant(a11, len); InitQuadrant(a12, len); InitQuadrant(a21, len); InitQuadrant(a22, len);
	InitQuadrant(b11, len); InitQuadrant(b12, len); InitQuadrant(b21, len); InitQuadrant(b22, len);
	InitQuadrant(c11, len); InitQuadrant(c12, len); InitQuadrant(c21, len); InitQuadrant(c22, len);

	for (i = 0; i < len; ++i) {
		partition(a, len, i, &a11, &a12, &a21, &a22);
		partition(b, len, i, &b11, &b12, &b21, &b22);
		partition(c, len, i, &c11, &c12, &c21, &c22);
	}

	/* create temporary matrices to hold the values of the intermediary
	 * sums and multiple operations */
	struct matrix s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, temp1, temp2;
	InitMatrix(s1, len); InitMatrix(s2, len); InitMatrix(s3, len);
	InitMatrix(s4, len); InitMatrix(s5, len); InitMatrix(s6, len);
	InitMatrix(s7, len); InitMatrix(s8, len); InitMatrix(s9, len);
	InitMatrix(s10, len); InitMatrix(temp1, len); InitMatrix(temp2, len);

	/* calculate each of the sums necessary for the algorithm */
	matrix_operation(&b12, &b22, &s1,  MATRIX_DIFFERENCE);
	matrix_operation(&a11, &a12, &s2,  MATRIX_SUM);
	matrix_operation(&a21, &a22, &s3,  MATRIX_SUM);
	matrix_operation(&b21, &b11, &s4,  MATRIX_DIFFERENCE);
	matrix_operation(&a11, &a22, &s5,  MATRIX_SUM);
	matrix_operation(&b11, &b22, &s6,  MATRIX_SUM);
	matrix_operation(&a12, &a22, &s7,  MATRIX_DIFFERENCE);
	matrix_operation(&b21, &b22, &s8,  MATRIX_SUM);
	matrix_operation(&a11, &a21, &s9,  MATRIX_DIFFERENCE);
	matrix_operation(&b11, &b12, &s10, MATRIX_SUM);

	/* create temporary matrices to hold the values of the intermediary
	 * products needed by the algorithm */
	struct matrix p1, p2, p3, p4, p5, p6, p7;
	InitMatrix(p1, len); InitMatrix(p2, len); InitMatrix(p3, len);
	InitMatrix(p4, len); InitMatrix(p5, len); InitMatrix(p6, len);
	InitMatrix(p7, len);

	/* the 7 recursive calls */
	matrix_multiply(&a11, &s1, &p1);
	matrix_multiply(&s2, &b22, &p2);
	matrix_multiply(&s3, &b11, &p3);
	matrix_multiply(&a22, &s4, &p4);
	matrix_multiply(&s5, &s6, &p5);
	matrix_multiply(&s7, &s8, &p6);
	matrix_multiply(&s9, &s10, &p7);

	/* building each quadrant of the resulting matrix */
	/* C11 = P5 + P4 - P2 + P6 */
	matrix_operation(&p5, &p4, &temp1, MATRIX_SUM);
	matrix_operation(&temp1, &p2, &temp2, MATRIX_DIFFERENCE);
	matrix_operation(&temp2, &p6, &c11, MATRIX_SUM);

	/* C12 = P1 + P2 */
	matrix_operation(&p1, &p2, &c12, MATRIX_SUM);

	/* C21 = P3 + P4 */
	matrix_operation(&p3, &p4, &c21, MATRIX_SUM);

	/* C22 = P5 + P1 - P3 - P7 */
	matrix_operation(&p5, &p1, &temp1, MATRIX_SUM);
	matrix_operation(&temp1, &p3, &temp2, MATRIX_DIFFERENCE);
	matrix_operation(&temp2, &p7, &c22, MATRIX_DIFFERENCE);

	/* at this point, the resulting C matrix should be correctly
	 * built. Free temporary data structures and terminate. */
	matrix_destroy(&s1); matrix_destroy(&s2); matrix_destroy(&s3);
	matrix_destroy(&s4); matrix_destroy(&s5); matrix_destroy(&s6);
	matrix_destroy(&s7); matrix_destroy(&s8); matrix_destroy(&s9);
	matrix_destroy(&s10); matrix_destroy(&temp1); matrix_destroy(&temp2);
	matrix_destroy(&p1); matrix_destroy(&p2); matrix_destroy(&p3);
	matrix_destroy(&p4); matrix_destroy(&p5); matrix_destroy(&p6);
	matrix_destroy(&p7);

	free(a11.data); free(a12.data); free(a21.data); free(a22.data);
	free(b11.data); free(b12.data); free(b21.data); free(b22.data);
	free(c11.data); free(c12.data); free(c21.data); free(c22.data);

	return 0;
}

int
matrix_traverse(struct matrix *m, int (*fn)(double el, long row, long col))
{
	ErrorCase(m == NULL, EINVAL, -1);
	ErrorCase(m->size <= 0, EINVAL, -1);

	int retval;
	long i, j;

	for (i = 0; i < m->size; ++i) {
		for (j = 0; j < m->size; ++j) {
			retval = fn(m->data[i][j], i, j);
			if (retval != 0)
				return retval;
		}
	}

	return 0;
}

int matrix_destroy(struct matrix *m)
{
	ErrorCase(m == NULL, EINVAL, -1);
	ErrorCase(m->size <= 0, EINVAL, -1);

	long i;
	for (i = 0; i < m->size; ++i)
		free(m->data[i]);

	free(m->data);
	return 0;
}
