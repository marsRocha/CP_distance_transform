#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void main(int argc, char* argv[]){ 

    char* filename = argv[1];
    
    FILE* file;

    if ((file = fopen(filename,"r")) == NULL) {
	    printf ("Cannot open file");
        exit(EXIT_FAILURE);
    }

    float sum = 0;
    int lines = 20;

    for (int i = 0; i < lines; i++)
    {
        float num;
        fscanf(file, "%f", &num);
        sum += num;
    }

    sum = sum/lines;

    printf("%f", sum);
}
