#ifndef _POSIX_C_SOURCE
#  define _POSIX_C_SOURCE 2
#endif

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <limits.h>
#ifndef LONG_MIN
#  include <linux/limits.h>
#endif

static int ifd = STDIN_FILENO,
           ofd = STDOUT_FILENO;

static char *separator = "\n";

extern char *progname;

static int
read_args(int argc, char *argv[]) {
  int opt;

  opterr = 0;
  while ((opt = getopt(argc, argv, "i:o:s:")) != -1) {
    switch (opt) {
      case 'i':
        ifd = open(optarg, O_RDONLY);
        if (ifd == -1) {
          return -1;
        }
        break;

      case 'o':
        ofd = open(
          optarg,
          O_WRONLY | O_CREAT | O_TRUNC,
          S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH
        );

        if (ofd == -1) {
          return -1;
        }
        break;

      case 's':
        separator = optarg;
        break;

      case '?':
        errno = EINVAL;
        return -1;
        break;
    }
  }

  return 0;
}

void
dataset_usage(void) {
  printf("Usage:\n\
    $ %s [-i <file>] [-o <file>] [-s <separator>>]\n\
    \t-i - input file\n\
    \t-o - output file\n\
    \t-s - separator: used to get numbers from input and to print the result\n", progname);
}

int
dataset_init(int argc, char *argv[]) {
  return read_args(argc, argv);
}

int
dataset_get(int *set, int const size) {
  long data, bufsiz, total_read;
  int i;
  ssize_t num_read;
  char *buf,
       *num,
       *saveptr,
       *endptr;


  bufsiz = 100 * BUFSIZ;
  buf = malloc(bufsiz);
  if (buf == NULL) {
    return -1;
  }

  i = total_read = 0;
  while ((num_read = read(ifd, buf + total_read, bufsiz - total_read)) > 0) {
    total_read += num_read;
  }

  if (num_read == -1) {
    return -1;
  }

  num = strtok_r(buf, separator, &saveptr);

  while (num != NULL) {
    data = strtol(num, &endptr, 10);

    /* test for invalid number */
    if (data == LONG_MIN || data == LONG_MAX || endptr == num) {
      errno = EINVAL;
      return -1;
    }

    set[i] = data;

    num = strtok_r(NULL, separator, &saveptr);
    ++i;

    if (i == size) {
      errno = ENOMEM;
      return -1;
    }
  }

  free(buf);

  return i;
}

int
dataset_print(int const *set, int const size) {
  int i, sstr;
  char buf[BUFSIZ];

  for (i = 0; i < size; ++i) {
    sstr = snprintf(buf, BUFSIZ, "%d%s", set[i], separator);
    if (write(ofd, buf, sstr) == -1) {
      return -1;
    }
  }

  return size;
}
