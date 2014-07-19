#include <stdio.h>
#include <stdlib.h>

void
pexit(char const *id) {
  perror(id);
  exit(EXIT_FAILURE);
}
