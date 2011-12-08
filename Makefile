C = gcc
CFLAGS = -Wall -pedantic -std=gnu99 -g
LDFLAGS =

OBJ = wavereader.o test.o

all: wread

wread: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

%.o: %.c
	@echo CC $@
	@${CC} -o $@ -c ${CFLAGS} $<

clean:
	@echo cleaning up...
	@rm *.o wread

.PHONY: all clean
