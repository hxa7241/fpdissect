#!/bin/bash


# --- using: LLVM-GCC 4.2 or GCC 4 ---

/Developer/usr/bin/llvm-gcc -c -x c -ansi -std=iso9899:199409 -pedantic -O3 -arch x86_64 -Wall -Wextra -Wcast-align -Wwrite-strings -Wpointer-arith -Wredundant-decls -Wdisabled-optimization $1.c

/Developer/usr/bin/llvm-gcc -arch x86_64 -o $1 $1.o

rm $1.o


exit
