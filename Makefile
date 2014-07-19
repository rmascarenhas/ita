CC=c99
CFLAGS=-I. -Wall -Wextra -O3

ODIR=shared

_OBJ = utils.o dataset.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(ODIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

build: $(OBJ)
	$(CC) -o $(PROG) $^ $(CFLAGS) $(PROG).c

run: build
	./$(PROG)

.PHONY: clean

clean:
	@rm -fv $(ODIR)/*.o
	@find . -path './.*' -prune -o -type f -executable -exec rm -v {} \;
