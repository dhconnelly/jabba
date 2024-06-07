CC=gcc
CFLAGS=-ansi -Wall -Werror
TARGETS=disasm

.PHONY: all clean

all: $(TARGETS)

clean:
	rm -f *.o $(TARGETS)

disasm: disasm.o logging.o buffer.o result.o parse.o
	$(CC) $(CFLAGS) $^ -o $@

logging.o: logging.c logging.h
	$(CC) $(CFLAGS) -c $< -o $@

result.o: result.c result.h
	$(CC) $(CFLAGS) -c $< -o $@

parse.o: parse.c parse.h result.h class.h
	$(CC) $(CFLAGS) -c $< -o $@

buffer.o: buffer.c buffer.h result.h
	$(CC) $(CFLAGS) -c $< -o $@

disasm.o: disasm.c buffer.h logging.h class.h parse.h
	$(CC) $(CFLAGS) -c $< -o $@
