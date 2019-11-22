#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h> // needed for isspace();
#include <omp.h>

#include "trabalho.h"

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })


pgm readPgmFile(char* filename){

    FILE* pgmFile;
    pgm data = malloc(sizeof(pgm));

    if ((pgmFile = fopen(filename,"r")) == NULL) {
	    printf ("Cannot open file");
        exit(EXIT_FAILURE);
    }	
    
    //check version
    char version[2];
    fgets(version, sizeof(version), pgmFile);
    if (strcmp(version, "P5") == 0) {
        printf("Wrong file type!\n");
        exit(EXIT_FAILURE);
    }
    
    fgetc(pgmFile);
    skipComments(pgmFile);
    fscanf(pgmFile, "%d", &(data->cols));
    skipComments(pgmFile);
    fscanf(pgmFile, "%d", &(data->rows));
    skipComments(pgmFile);
    fscanf(pgmFile, "%d", &(data->max_gray));
    fgetc(pgmFile);


    data->pData = (int*)malloc(sizeof(int) * data->cols * data->rows);
    int pInt;
    // allocate information from file
    for (int row = 0; row < data->rows; row++) {
        for (int col = 0; col < data->cols; col++) {
            fscanf(pgmFile,"%d", &pInt);
            if(pInt != 0)
                data->pData[row+col*data->rows] = 65,536;
            else data->pData[row+col*data->rows] = pInt;
        }
    }

    fclose(pgmFile);
    return data;
}

void writePgmFile(char* filename, pgm newImg){

    FILE* pgmFile;

    if ((pgmFile = fopen(filename,"w")) == NULL) {
	    printf ("Cannot open file");
        exit(EXIT_FAILURE);
    }

    //write header
    fprintf(pgmFile, "P2\n");
    fprintf(pgmFile, "%d %d\n", newImg->cols, newImg->rows);
    fprintf(pgmFile, "%d\n", newImg->max_gray);

    //write data
    for (int row = 0; row < newImg->rows; row++) {
        for (int col = 0; col < newImg->cols; col++) {
            int pInt = newImg->pData[row+col*newImg->rows];
            fprintf(pgmFile, "%d\n",pInt);
        }
    }
    fclose(pgmFile);
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
    while( c == '#' || isspace(c) );
    if( c != EOF && ungetc(c,f) == EOF ){
        printf("Unable to 'ungetc' while reading PGM file.");
        exit(EXIT_FAILURE);   
    }
} 


int main(){ 

    double wtime = omp_get_wtime ( );

    char* filename = "sdr.pgm";
    char* ofilename = "outImage.pgm";
    pgm pgmData;  

    pgmData = readPgmFile(filename);

    pgm pgmNew = pgmData;
    int pInt;
    for (int row = 1; row < pgmData->rows-1; row++) {
        for (int col = 1; col < pgmData->cols-1; col++) {
            pInt = pgmData->pData[row+col*pgmData->rows];

            if(pInt == 65,536){
                int minA = min(pgmData->pData[(row-1)+col*pgmData->rows],pgmData->pData[(row+1)+col*pgmData->rows]);
                int minB = min(pgmData->pData[row+(col-1)*pgmData->rows],pgmData->pData[row+(col+1)*pgmData->rows]);
                int minC = min(minB,minA);
                pgmNew->pData[row+col*pgmData->rows] = minC + 1;
                if(pgmNew->max_gray < minC)
                    pgmNew->max_gray = minC;  
            }
        }
    }

    for (int row = pgmData->rows-1; row > 1; row--) {
        for (int col = pgmData->cols-1; col > 1; col--) {
            pInt = pgmData->pData[row+col*pgmData->rows];

            if(pInt == 65,536){
                int minA = min(pgmData->pData[(row-1)+col*pgmData->rows],pgmData->pData[(row+1)+col*pgmData->rows]);
                int minB = min(pgmData->pData[row+(col-1)*pgmData->rows],pgmData->pData[row+(col+1)*pgmData->rows]);
                pgmNew->pData[row+col*pgmData->rows] = min(minB,minA) + 1;
            }
        }
    }
    writePgmFile(ofilename, pgmNew);

    wtime = omp_get_wtime() - wtime;
    printf("Wallclock time = %f\n", wtime);
    printf("Normal end of execution.\n");
}