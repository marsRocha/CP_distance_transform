#include<omp.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_VALUE 65535
int MAX_THREADS;

int columns;
int rows;
int max_gray;
int* pixelData;

double wtime;

void skipComments(FILE* f){
    int c;
    do
    {
        while(isspace(c=getc(f))); // skip spaces
        if(c=='#') //skip comments
            while( c!='\n' && c!='\r' && c!=EOF )
                c=getc(f);
    }
    while( c == '#' || isspace(c));
    if( c != EOF && ungetc(c,f) == EOF ){
        printf("Unable to 'ungetc' while reading PGM file.");
        exit(EXIT_FAILURE);   
    }
} 

int main(int argc, char* argv[]){ 

    char* filename = argv[1];
    MAX_THREADS = atoi(argv[2]);

    FILE* pgmFile;

    if ((pgmFile = fopen(filename,"r")) == NULL) {
	    printf ("Cannot open file");
        exit(EXIT_FAILURE);
    }	

    fgetc(pgmFile);
    fgetc(pgmFile);
    skipComments(pgmFile);
    fscanf(pgmFile, "%d", &columns);
    skipComments(pgmFile);
    fscanf(pgmFile, "%d", &rows);
    skipComments(pgmFile);
    fscanf(pgmFile, "%d", &max_gray);
    fgetc(pgmFile);
    
    pixelData = (int*)malloc(sizeof(int) * columns * rows);

    int pInt;

    wtime = omp_get_wtime();
    #pragma omp parallel for schedule(dynamic) num_threads(MAX_THREADS)
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < columns; col++) {
            fscanf(pgmFile,"%d", &pInt);
            if(pInt != 0)
                pixelData[col + row * columns] = MAX_VALUE;
            else pixelData[col + row * columns] = 0;
        }
    }
    max_gray = 0;
    fclose(pgmFile);
    wtime = omp_get_wtime() - wtime;

    printf("%f\n", wtime);
}