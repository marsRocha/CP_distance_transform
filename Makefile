compile :
	gcc -O3 -std=c99 -fopenmp dynamic.c -o work -lm

clear :
       	rm work
	rm Cpteste.*
