#!/bin/bash
cat $1 | ../cc-i386 >tmp.s
nasm -felf tmp.s -o tmp.o
gcc -m32 tmp.o -o a.out
rm tmp.s
rm tmp.o 
