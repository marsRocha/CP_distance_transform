compile :
	gcc -O3 -std=c99 -fopenmp seq.c -o work -lm

clear :
       	rm work
		rm Cpteste.*