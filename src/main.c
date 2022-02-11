#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>


#define FILENAME "testFile.txt"
int main()
{
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    char *ptr;
    long * number = NULL;
    int numElements = 0;

    fp = fopen(FILENAME, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        numElements++;
        long temp = strtol(line, &ptr, 16);
        number = realloc(number, numElements*sizeof(long));
        number[numElements-1] = temp;
        printf("Retrieved line of length %zu:\n", read);
        printf("Hex value: %s", line);
        printf("int value: %ld\n",number[numElements-1]);

    }
    numElements--;
    printf("\n\n");
    fclose(fp);
    if (line){
        free(line);
    }
    for(int i = 0; i < numElements; i++)
    {
        printf("element %d: %ld\n", i, number[i]);
    }
    
    exit(EXIT_SUCCESS);
    
}

