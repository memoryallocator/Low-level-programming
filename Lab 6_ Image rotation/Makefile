CFLAGS=-std=c18 -Wall -pedantic -Isrc/ -ggdb -Wextra -Werror -DDEBUG
CC=gcc

LDLIBS=-lm

all: main.o

bmp.o: bmp.c util.o
	$(CC) -c $(CFLAGS) $< -o $@ $(LDLIBS)

matrix.o: matrix.c
	$(CC) -c $(CFLAGS) $< -o $@ $(LDLIBS)

bgr_image_transformations.o: bgr_image_transformations.c matrix.o
	$(CC) -c $(CFLAGS) $< -o $@ $(LDLIBS)

test.o: test.c
	$(CC) -c $(CFLAGS) $< -o $@ $(LDLIBS)

util.o: util.c
	$(CC) -c $(CFLAGS) $< -o $@ $(LDLIBS)

main.o: main.c
	$(CC) -c $(CFLAGS) $< -o $@ $(LDLIBS)

rotate: main.o util.o bmp.o test.o bgr_image_transformations.o matrix.o
	$(CC) $^ -o $@ $(LDLIBS)

clean:
	rm -f main.o util.o bmp.o util.o test.o bgr_image_transformations.o matrix.o main