CC=c99
CFLAGS=-I. -g -Wall -Wextra -O2

ODIR=shared

_OBJ = utils.o dataset.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(ODIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

build: $(OBJ)
	$(CC) -o $(PROG) $^ $(CFLAGS) $(PROG).c

run: build
	./$(PROG)

check:
	bash $(TEST)

clean:
	@rm -fv $(ODIR)/*.o
	@find . -path './.*' -prune -o -type f -executable -exec rm -v {} \;

.PHONY: build run check clean
