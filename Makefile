CC = gcc #GCC compiler
CFLAGS = -O3 -std=c11 -Wall -Wextra #Optimizes code, uses C11 standard, and enables all warnings
all: array_max #Builds the array_max
array_max: src/array_max.c #Specifies the source files and header files needed to build array_max
	$(CC) $(CFLAGS) -o array_max src/array_max.c
clean: #Deletes the array_max
	rm -f array_max
