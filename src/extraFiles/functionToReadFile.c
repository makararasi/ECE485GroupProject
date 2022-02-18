#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct address_s
{
    int n;
    long addr;
} address_t, *addressPtr_t;

#define FILENAME "testFile.txt"
// pass filename with list of hex addresses and a pointer
// to an int for the size of the array will return an
// array of long integers
address_t *read_file(const char *filename, int *size);



int main()
{
    
    int size; // Size of array read
    address_t *array = read_file(FILENAME, &size);

    // just printing for testing
    for (int i = 0; i < size; i++)
    {
        printf("n %d: %d\n", i, array[i].n);
        printf("address %d: %ld\n", i, array[i].addr);
    }
    printf("Number of Elements: %d\n", size);

    exit(EXIT_SUCCESS);
}

address_t *read_file(const char *filename, int *size)
{
    FILE *fp;          // declare filepointer
    char *line = NULL; // input line from file variable
    size_t len = 0;
    ssize_t read;        // number of characters read on the line
    char *ptr;           // this isn't really used. But holds chars that aren't in the number
    address_t *number = NULL; // the array of addresses read
    int numElements = 0; // size of the array

    fp = fopen(FILENAME, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
    // read to the end of the file line by line
    while ((read = getline(&line, &len, fp)) != -1)
    {
        

        numElements++;
        number = realloc(number, numElements * sizeof(address_t));
        char temp[2];
        char temp2[10];
        strncpy(temp, line, 2);
        
        number[numElements - 1].n = atoi(temp);
        strcpy(temp2,line+2);
        
        number[numElements - 1].addr = strtol(temp2, &ptr, 16);
        
         
    }
    fclose(fp);
    if (line)
    {
        free(line);
    }
    *size = numElements;
    return number;
}