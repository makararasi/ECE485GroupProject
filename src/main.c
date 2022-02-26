#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "include/Defines.h"
#include <inttypes.h>

int main()
{
	long N, address;
	input_addr given_addr;
	uint16_t index_sel, tag_sel;

	misses = 27;
	hits = 40;

	memset(instruction_cache, 0, CACHE_SIZE_INSTR); // it will set our cache to all zeros to start with.
	memset(data_cache, 0, CACHE_SIZE_DATA);

	int size; // Size of array read
	addressPtr_t array = read_file(FILENAME, &size);
	// assigning input address into the structure. this is done after we read data from the file and when we have got N and address separately.
	given_addr.addr_store = address;
	for (int i = 0; i < size; i++)
	{
		char temp[33]; // 32 bits +1 for null terminator
		given_addr.addr_store = array[i].addr;
		printf("element: %d\n", i);
		printf("N bit: %d\n", array[i].n);
		printf("full address HEX: %x\n", given_addr.addr_store);
		itoa(given_addr.bits.byte, temp, 2);
		printf("byte BIN: %s\n", temp);
		printf("index HEX: %x\n", given_addr.bits.index);
		itoa(given_addr.bits.index, temp, 2);
		printf("index BIN: %s\n", temp);
		printf("tag HEX: %x\n", given_addr.bits.tag);
		itoa(given_addr.bits.tag, temp, 2);
		printf("tag BIN: %s\n", temp);
		
	}
	print_hit_miss();
	// after we have store our address in given_addr which is of type input_addr.  now we are extracting the desired bits we need.
	index_sel = given_addr.bits.index; // index_sel will get the bits from 6-19.
	tag_sel = given_addr.bits.tag;	   // tag_sel will get the bits 20-31.

	return 0;
}
