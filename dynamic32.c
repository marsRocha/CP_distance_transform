#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <omp.h>

#define MAX_VALUE 65535
int MAX_THREADS;

int columns;
int rows;
int max_gray;
int* pixelData;

int* mpixelData;

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

    int* cupthree;

    int haswhite = 1;
    int pInt;

    wtime = omp_get_wtime();
    
    while(haswhite == 1){
        int hadwhite = 0;

        #pragma omp parallel for schedule(dynamic,32) num_threads(MAX_THREADS) private(pInt)
        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < columns; col++) {
                pInt = pixelData[col + row * columns];
                //se for branco
                if(pInt == MAX_VALUE){
                    hadwhite = 1;
                    int minNei = MAX_VALUE;
                    //percorre os vizinhos
                    for (int r = row-1; r <= row+1; r++) {
                        for (int c = col-1; c <= col+1; c++) {
                            //se nao for o pixel onde estou ou existir no array
                            if((r != row && c != col) || (r <= -1 || r >= rows + 1 || c <= -1 || c >= columns + 1)){
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
                    else
                    {       
                        mpixelData[col + row * columns] = pInt;
                    }
                }
                else
                {
                    mpixelData[col + row * columns] = pInt;
                }
            }
        }
        cupthree = pixelData;
        pixelData = mpixelData;
        mpixelData = cupthree;

        haswhite = hadwhite;
    }
    wtime = omp_get_wtime() - wtime;
}


int main(int argc, char* argv[]){ 

    char* filename = argv[1];
    char* ofilename = "outImage.pgm"; 
    MAX_THREADS = atoi(argv[2]);

    //reads the distance tranform of the original image
    readPgmFile(filename);

    mpixelData = (int*)malloc(sizeof(int) * columns * rows);

    //calculates the distance transform from the sourced image
    transform();

    //writes the distance transform of the original image
    writePgmFile(ofilename);

    printf("%f\n", wtime);
}
