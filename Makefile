CC = gcc
CFLAGS = -Wall -pedantic -std=gnu99 -g
LDFLAGS = -lm

OBJ = wavereader.o test.o stat.o

all: wread

wread: $(OBJ)
	@echo CC -o $@
	@$(CC) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.c
	@echo CC $@
	@$(CC) -o $@ -c $(CFLAGS) $<

clean:
	@echo cleaning up...
	@rm *.o wread

run:
	@./wread ./wavs/Square1.wav

.PHONY: all clean
