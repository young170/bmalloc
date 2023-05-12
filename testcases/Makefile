all: ../bmalloc.h ../bmalloc.c test_freeall.c test_struct.c
	gcc -o test_freeall test_freeall.c ../bmalloc.c -I../
	gcc -o test_struct test_struct.c ../bmalloc.c -I../ -fpack-struct

clean:
	rm -rf test_freeall test_struct bmalloc.o
