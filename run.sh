#!/bin/bash
make all || exit 1
./cc-i386 <test.c >tmp.s || exit 1
nasm -felf -F dwarf -g tmp.s -o tmp.o
gcc -m32 tmp.o -o tmp.out
./tmp.out
echo $?