#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h> //bool
#include <ctype.h> // needed for isspace();
#include <omp.h>

#define MAX_VALUE 65535

int MAX_THREADS = 2;

int columns;
int rows;
int max_gray;
int* pixelData;

//To record time of execution
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
}

void writePgmFile(char* filename){

    FILE* pgmFile;

    pgmFile = fopen(filename,"w");

    //write header
    fprintf(pgmFile, "P2\n");
    fprintf(pgmFile, "%d %d\n", columns, rows);
    fprintf(pgmFile, "%d\n", max_gray);

    //write data
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < columns; col++) {   
            int pInt = pixelData[col + row * columns];
            fprintf(pgmFile, "%d ",pInt);
        }
        fprintf(pgmFile, "\n");
    }
    fclose(pgmFile);
}

void transform(){

    int* mpixelData = (int*)malloc(sizeof(int) * columns * rows);
    memcpy(mpixelData, pixelData, sizeof(int) * columns * rows);

    bool haswhite = true;
    int pInt;
    double totaltime = 0;

    omp_set_num_threads(MAX_THREADS);
    //wtime = omp_get_wtime();
    
    while(haswhite){
        bool hadwhite = false;

        #pragma omp parallel for schedule(guided)
        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < columns; col++) {
                pInt = pixelData[col + row * columns];
                //se for branco
                if(pInt == MAX_VALUE){
                    hadwhite = true;
                    int minNei = MAX_VALUE;
                    //percorre os vizinhos
                    for (int r = row-1; r <= row+1; r++) {
                        for (int c = col-1; c <= col+1; c++) {
                            //se nao for o pixel onde estou ou existir no array
                            if(r != row && c != col || (r <= -1 || r >= rows + 1 || c <= -1 || c >= columns + 1)){
                                int neiPixel = pixelData[c + r * columns];
                                if(neiPixel < minNei)
                                    minNei = neiPixel;
                            }
                        }
                    }
                    if(minNei < MAX_VALUE){
                        mpixelData[col + row * columns] = minNei + 1;
                        if(max_gray < minNei + 1)
                            max_gray = minNei + 1;
                    }
                }
            }
        }
        wtime = omp_get_wtime();
        haswhite = hadwhite;
        memcpy(pixelData, mpixelData,sizeof(int) * columns * rows);
        wtime = omp_get_wtime() - wtime;
        totaltime += wtime;
        //printf("%f\n", wtime);
    }
    printf("%f\n", totaltime);
}


int main(int argc, char* argv[]){ 

    char* filename = argv[1];
    char* ofilename = "outImage.pgm"; 

    readPgmFile(filename);

    //calculates the distance transform from the sourced image
    transform();

    //writes the distance tranform of the original image
    writePgmFile(ofilename);

    //printf("%f\n", wtime);
}