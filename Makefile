CC=gcc
CFLAGS=-ansi -Wall -Werror
TARGETS=disasm

.PHONY: all clean

all: $(TARGETS)

clean:
	rm -f *.o $(TARGETS)

disasm: disasm.o logging.o buffer.o result.o
	$(CC) $(CFLAGS) $^ -o $@

result.o: result.c result.h
	$(CC) $(CFLAGS) -c $< -o $@

logging.o: logging.c logging.h
	$(CC) $(CFLAGS) -c $< -o $@

buffer.o: buffer.c buffer.h result.h
	$(CC) $(CFLAGS) -c $< -o $@

disasm.o: disasm.c buffer.h logging.h class.h
	$(CC) $(CFLAGS) -c $< -o $@
