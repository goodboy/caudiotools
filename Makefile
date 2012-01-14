CC = gcc
CFLAGS = -Wall -pedantic -std=gnu99 -g
LDFLAGS = -lm	

OBJ = wavereader.o test.o stats.o 

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
	@./wread ./wavs/Pulse2.wav

mem:
	@valgrind --leak-check=yes ./wread ./wavs/Pulse1.wav 
 
	
.PHONY: all clean
