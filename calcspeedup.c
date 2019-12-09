#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>


char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void main(int argc, char* argv[]){ 

    char* inputDir = argv[1];
    
    FILE *currentFile;

    currentFile = fopen(inputDir, "r");
    if (currentFile == NULL)
    {
        fprintf(stderr, "Error : Failed to open entry file - %s\n", strerror(errno));
        printf ("Cannot open file");
        exit(EXIT_FAILURE);
    }
    float sum = 0;
    int lines = 35;
    int tCount = 0;
    int seqI = 0;
    float seq[5]= { 0.008549, 0.065551, 0.584362, 4.534909, 35.994884 };
    for (int i = 0; i < lines; i++)
    {
        float speed;
        float result;
        fscanf(currentFile, "%f", &speed);
        result = seq[seqI]/speed;
        printf("%f / %f = %f\n", seq[seqI],speed, result);
        tCount++;
        if(tCount >= 7){
            tCount = 0;
            seqI++;
            if(seqI>=5)
            seqI = 0;
        }
    }
    fclose(currentFile);
}