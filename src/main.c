#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>


#define FILENAME "testFile.txt"
// pass filename with list of hex addresses and a pointer 
// to an int for the size of the array will return an 
// array of long integers
long *read_file(const char *filename, int *size);


int main()
{
    int size;       // Size of array read
    long * array = read_file(FILENAME, &size);


    //just printing for testing
    for (int i = 0; i < size; i++)
    {
        printf("element %d: %ld\n", i, array[i]);
    }
    printf("Number of Elements: %d\n", size);

    exit(EXIT_SUCCESS);
}

long *read_file(const char *filename, int *size)
{
    FILE *fp;   // declare filepointer
    char *line = NULL;  // input line from file variable
    size_t len = 0;     
    ssize_t read;   // number of characters read on the line
    char *ptr;  // this isn't really used. But holds chars that aren't in the number
    long *number = NULL;    // the array of addresses read
    int numElements = 0;    //size of the array

    fp = fopen(FILENAME, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
    // read to the end of the file line by line
    while ((read = getline(&line, &len, fp)) != -1)
    {
        numElements++;
        long temp = strtol(line, &ptr, 16);
        number = realloc(number, numElements * sizeof(long));
        number[numElements - 1] = temp;
    }
     fclose(fp);
    if (line)
    {
        free(line);
    }
    *size = numElements;
    return number;
}