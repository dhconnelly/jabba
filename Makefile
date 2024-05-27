CC=gcc
CFLAGS=-ansi -Wall -Werror
TARGETS=disasm

.PHONY: all clean

all: $(TARGETS)

clean:
	rm -f *.o $(TARGETS)

disasm: disasm.o
	$(CC) $(CFLAGS) $^ -o $@

disasm.o: disasm.c
	$(CC) $(CFLAGS) -c $< -o $@
