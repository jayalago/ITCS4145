CC = gcc #GCC compiler
CFLAGS = -O3 -std=c11 -Wall -Wextra #Optimizes code, uses C11 standard, and enables all warnings

all: array_max sweep #Builds the array_max and sweep_bandwidth

array_max: src/array_max.c #Specifies the source files and header files needed to build array_max
	$(CC) $(CFLAGS) -o array_max src/array_max.c

sweep: src/sweep_bandwidth.c
	$(CC) $(CFLAGS) -o sweep_bandwidth src/sweep_bandwidth.c

clean:
	rm -f array_max sweep_bandwidth

