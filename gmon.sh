#!/bin/bash
./cc <test.c >tmp.s
mv gmon.out gmon.sum
for i in {1..10} 
do
	./cc <test.c >tmp.s
	gprof -s ./cc gmon.out gmon.sum
done

gprof ./cc gmon.sum >analysis.txt
