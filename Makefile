CFLAGS = -O3 -lSDL2 -lm -mavx2 -mfma
CC=gcc
OBJECTS = render.o kernels.o random.o

all: nbody
	./nbody

debug: nbody
	valgrind ./nbody

nbody: $(OBJECTS) 
	$(CC) main.c $^ $(CFLAGS) -o $@

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $^ 

clean: 
	rm -f nbody
	rm -f *.o
	rm -f *.gch

.PHONY: nbody kernels clean debug all
