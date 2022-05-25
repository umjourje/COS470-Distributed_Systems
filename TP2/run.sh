#!/bin/bash
# gcc -o t semaphores.c -pthread

gcc -o ex1 threads.c -pthread

for k in 1 2 4 8 16 32 64 128 256
do
	echo "$k;10^7;"
	./ex1 10000000 $k
	
	echo "$k;10^8;"
	./ex1 100000000 $k
	
	echo "$k;10^9;"
	./ex1 1000000000 $k
done

# for idx in 1 2 3 4 5
# do
# 	echo "1,1: "
# 	./t 1 1
	
# 	echo "1,2: "
# 	./t 1 2
	
# 	echo "1,4: "
# 	./t 1 4

# 	echo "1,8: "
# 	./t 1 8

# 	echo "2,1: "
# 	./t 2 1

# 	echo "4,1: "
# 	./t 4 1

# 	echo "8,1: "
# 	./t 8 1

# done