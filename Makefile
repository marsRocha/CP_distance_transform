compile : 
	echo Compling
	gcc -O3 -fopenmp -o work forParallel.c
	echo Running
	./work de.pgm

clear : 
	rm work
