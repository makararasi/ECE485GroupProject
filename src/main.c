#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define FILENAME "testFile.txt"
#define SETS 16 * 1024            // Same number of sets for both cache.
#define WAYS_DATA 8               // ways for data cache
#define WAYS_INSTR 4              // ways for instruction cache
#define BYTES sizeof(stored_data) // these three extra lines are for our calculations.
#define CACHE_SIZE_DATA (SETS * WAYS_DATA * BYTES)
#define CACHE_SIZE_INSTR (SETS * WAYS_INSTR * BYTES)

long *read_file(const char *filename, int *size);
char *itoa(int value, char *result, int base);

typedef union // this union will have 32 bits memory space allocated to it
{
    struct __attribute__((__packed__))
    {
        u_int8_t snoop : 2;
        u_int8_t byte : 4;
        u_int16_t index : 14;
        u_int16_t tag : 12;
    } bits;
    u_int32_t addr_store; // we will store the retrived address from the file and will access them by the size we need (as given in the above structure).
} input_addr;

typedef struct __attribute__((__packed__)) // this structure will have 16 bits allocated to it.
{
    u_int16_t tag_store : 12;
    u_int8_t MESI : 2;
    u_int8_t reserved : 2;
} stored_data;

stored_data instruction_cache[SETS][WAYS_INSTR]; // to store instruction cache line data which are of type stored_data (structure).
stored_data data_cache[SETS][WAYS_DATA];         // to store data cache line .

int main()
{

    long N, address;
    input_addr given_addr;
    u_int8_t snoop_sel;
    u_int16_t index_sel, tag_sel;

    memset(instruction_cache, 0, CACHE_SIZE_INSTR); // it will set our cache to all zeros to start with.
    memset(data_cache, 0, CACHE_SIZE_DATA);

    // we can start our file open from here
    // once we get our N (traces operation) we can atore it in N variable to select which operation we need to do.
    // after N we will get our address which will be store in the format given bellow.
    int size; // Size of array read
    long *array = read_file(FILENAME, &size);

    // assigning input address into the structure. this is done after we read data from the file and when we have got N and address separately.
    // given_addr.addr_store = address;

    // This is just to test the loading of the bits in the data structure.
    // it works. 
    for (int i = 0; i < size; i++)
    {
        char temp[33]; // 32 bits +1 for null terminator
        given_addr.addr_store = array[i];
        printf("element: %d\n", i);
        printf("full address HEX: %x\n", given_addr.addr_store);
        printf("snoop HEX: %x\n", given_addr.bits.snoop);
        itoa(given_addr.bits.snoop, temp, 2);
        printf("snoop BIN: %s\n", temp);
        printf("byte HEX: %x\n", given_addr.bits.byte);
        itoa(given_addr.bits.byte, temp, 2);
        printf("byte BIN: %s\n", temp);
        printf("index HEX: %x\n", given_addr.bits.index);
        itoa(given_addr.bits.index, temp, 2);
        printf("index BIN: %s\n", temp);
        printf("tag HEX: %x\n", given_addr.bits.tag);
        itoa(given_addr.bits.tag, temp, 2);
        printf("tag BIN: %s\n", temp);
    }

    // after we have store our address in given_addr which is of type input_addr.  now we are extracting the desired bits we need.
    snoop_sel = given_addr.bits.snoop; // snoop_sel will get the bits LSB 0 and 1 position.
    index_sel = given_addr.bits.index; // index_sel will get the bits from 6-19.
    tag_sel = given_addr.bits.tag;     // tag_sel will get the bits 20-31.

    return 0;
}

long *read_file(const char *filename, int *size)
{
    FILE *fp;          // declare filepointer
    char *line = NULL; // input line from file variable
    size_t len = 0;
    int read;            // number of characters read on the line
    char *ptr;           // this isn't really used. But holds chars that aren't in the number
    long *number = NULL; // the array of addresses read
    int numElements = 0; // size of the array

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

// from: https://stackoverflow.com/questions/8257714/how-to-convert-an-int-to-string-in-c
/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
 */
char *itoa(int value, char *result, int base)
{
    // check that the base if valid
    if (base < 2 || base > 36)
    {
        *result = '\0';
        return result;
    }

    char *ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do
    {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + (tmp_value - value * base)];
    } while (value);

    // Apply negative sign
    if (tmp_value < 0)
        *ptr++ = '-';
    *ptr-- = '\0';
    while (ptr1 < ptr)
    {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}