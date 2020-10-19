#!/bin/bash
make all || exit 1
./cc-mod5 <test.c >tmp.s || exit 1
cat tmp.s
mod5asm tmp.s