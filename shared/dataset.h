#ifndef DATASET_H
#define DATASET_H

#ifndef _POSIX_C_SOURCE
#  define _POSIX_C_SOURCE 2
#endif

/* The functions defined here are meant to deal with getting a dataset from
 * the user and returning a dataset to the user, in a transparent way to the
 * caller.
 *
 * The user may specify three command line arguments:
 *
 *  -i <file>, causes the input data to be read from the given <file>
 *  -o <file>, causes the output data to be written to <file>
 *  -s <separator>, uses the given separator when reading and writing files.
 *
 *  In case no input or output file is specified, data is read from the command line
 *  and print to the
 *
 *  All functions return the number of elements in the buffer when successful,
 *  or -1 on error.
 *
 *  Before manipulating any dataset (either via `get_dataset' or `print_dataset')
 *  the caller must first call `init_dataset'.
 */

extern int init_dataset(int argc, char *argv[]);

extern int get_dataset(int *buf, int const size);

extern int print_dataset(int const *buf, int const size);

#endif
