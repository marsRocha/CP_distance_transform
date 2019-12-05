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
    
    DIR* FD;
    struct dirent* in_file;
    FILE    *currentFile;

    if (NULL == (FD = opendir (inputDir))) 
    {
        fprintf(stderr, "Error : Failed to open input directory - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    while ((in_file = readdir(FD))) 
    {        
        if (!strcmp (in_file->d_name, "."))
            continue;
        if (!strcmp (in_file->d_name, ".."))    
            continue;
        printf("%s\n", in_file->d_name);
        char* path = concat(inputDir,in_file->d_name);
        currentFile = fopen(path, "r");
        if (currentFile == NULL)
        {
            fprintf(stderr, "Error : Failed to open entry file - %s\n", strerror(errno));
            printf ("Cannot open file");
            exit(EXIT_FAILURE);
        }

        float sum = 0;
        int lines = 100;

        for (int i = 0; i < lines; i++)
        {
            float num;
            fscanf(currentFile, "%f", &num);
            sum += num;
        }

            sum = sum/lines;
            printf("%f\n", sum);
        fclose(currentFile);
    }
}