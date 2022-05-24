#!/bin/bash
gcc -o t semaphores.c -pthread
for idx in 1 2 3
do
	echo "1,1: "
	./t 1 1
	
	echo "1,2: "
	./t 1 2
	
	echo "1,4: "
	./t 1 4

	echo "1,8: "
	./t 1 8

	echo "2,1: "
	./t 2 1

	echo "4,1: "
	./t 4 1

	echo "8,1: "
	./t 8 1

done