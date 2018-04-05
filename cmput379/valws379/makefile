all:
	make valws379 --always-make

valws379:
	gcc -std=c99 -D_POSIX_C_SOURCE valws379.c -o valws379.bin
	gcc -std=c99 mergesort.c -o mergesort
	gcc -std=c99 heapsort.c -o heapsort
	gcc -std=c99 quicksort.c -o quicksort
clean: 
	rm valws379.bin mergesort
