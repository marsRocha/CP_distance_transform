#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h> //bool
#include <ctype.h> // needed for isspace();
#include <omp.h>

#define MAX_VALUE 65535
#define MAX_THREADS 2

int columns;
int rows;
int max_gray;
int* pixelData;

//To record time of execution
double wtime;

int min(int a, int b){
    if( a < b)
        return a;
    else
        return b;   
}

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

void readPgmFile(char* filename){

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
    // allocate information from file
    for (int col = 0; col < columns; col++) {
        for (int row = 0; row < rows; row++) {
            fscanf(pgmFile,"%d", &pInt);
            if(pInt != 0)
                pixelData[row + col * rows] = MAX_VALUE;
            else pixelData[row + col * rows] = 0;
        }
    }
    max_gray = 0;
    fclose(pgmFile);
}

void writePgmFile(char* filename){

    FILE* pgmFile;

    pgmFile = fopen(filename,"w");

    //write header
    fprintf(pgmFile, "P2\n");
    fprintf(pgmFile, "%d %d\n", columns, rows);
    fprintf(pgmFile, "%d\n", max_gray);

    //write data
    for (int col = 0; col < columns; col++) {
        for (int row = 0; row < rows; row++) {
            int pInt = pixelData[row + col * rows];
            fprintf(pgmFile, "%d ",pInt);
        }\
        fprintf(pgmFile, "\n");
    }
    fclose(pgmFile);
}

void transform(){

    int* mpixelData = (int*)malloc(sizeof(int) * columns * rows);
    memcpy(mpixelData, pixelData, sizeof(int) * columns * rows);

    bool haswhite = true;
    int pInt;

    wtime = omp_get_wtime();
    
    while(haswhite){
        bool hadwhite = false;

        omp_set_num_threads(MAX_THREADS);
        #pragma omp parallel for schedule(dynamic)
        for (int col = 0; col < columns; col++) {
            for (int row = 0; row < rows; row++) {
                pInt = pixelData[row + col * rows];
                //se for branco
                if(pInt == MAX_VALUE){
                    hadwhite = true;
                    int minNei = MAX_VALUE;
                    //percorre os vizinhos
                    for (int c = col-1; c <= col+1; c++) {
                        for (int r = row-1; r <= row+1; r++) {
                            //se nao for o pixel onde estou ou existir no array
                            if(r != row && c != col || (r <= -1 || r >= rows + 1 || c <= -1 || c >= columns + 1)){
                                int neiPixel = pixelData[r + c * rows];
                                minNei = min(neiPixel,minNei);
                            }
                        }
                    }
                    if(minNei < MAX_VALUE){
                        mpixelData[row + col * rows] = minNei + 1;
                        if(max_gray < minNei + 1)
                            max_gray = minNei + 1;
                    }
                }
            }
        }
        haswhite = hadwhite;
        memcpy(pixelData, mpixelData,sizeof(int) * columns * rows);
    }
    wtime = omp_get_wtime() - wtime;
}


int main(int argc, char* argv[]){ 

    char* filename = argv[1];
    char* ofilename = "outImage.pgm"; 

    readPgmFile(filename);

    //calculates the distance transform from the sourced image
    transform();

    //writes the distance tranform of the original image
    writePgmFile(ofilename);

    printf("%f\n", wtime);
}