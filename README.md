# OS_HW3

### Instructions
1. build test cases
```
$ make
gcc -o test1 test1.c bmalloc.c
gcc -o test2 test2.c bmalloc.c
gcc -o test3 test3.c bmalloc.c 
```
2. run desired test case
```
$ ./test1
$ ./test2
$ ./test3
```

### Run addtional test cases
1. move to the *testcases* directory
```
$ cd testcases/
```
2. build test cases
```
$ make
gcc -o test_freeall test_freeall.c ../bmalloc.c -I../
gcc -o test_struct test_struct.c ../bmalloc.c -I../ -fpack-struct
```
3. run desired test case
```
$ ./test_freeall
$ ./test_struct
```