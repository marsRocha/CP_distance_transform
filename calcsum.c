#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void main(int argc, char* argv[]){ 

    char* filename = argv[1];
    char* ofilename = "outImage.pgm"; 
    
    FILE* file;

    if ((file = fopen(filename,"r")) == NULL) {
	    printf ("Cannot open file");
        exit(EXIT_FAILURE);
    }

    double sum = 0;

    for (int i = 0; i < 2; i++)
    {
        double num;
        fgetc(file);
        fscanf(file, "%lf", &num);
        printf("%lf", num);
        sum += num;
    }

    sum = sum/100;

    printf("%lf", sum);
}
