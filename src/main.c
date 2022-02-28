#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "Defines.h"
#include <inttypes.h>

int main()
{
	int N, mode;
	int size; // Size to read
	input_addr given_addr;
	uint16_t index_sel, tag_sel;

	// program requires 2 modes.  Here we ask the user to choose which mode
	// mode can only be 0 or 1
	bool modeSel = false;
	while (!modeSel)
	{

		printf("Enter the desired mode :- ");
		scanf("%d", &mode);
		if (mode == 0 || mode == 1)
		{
			printf("the desired mode is :- %d\n", mode);
			modeSel = true;
		}
		else
		{
			printf("The mode can eiter be 0 or 1\n  Try again.\n");
		}
	}
	memset(instruction_cache, 0, CACHE_SIZE_INSTR); // it will set our cache to all zeros to start with.
	memset(data_cache, 0, CACHE_SIZE_DATA);
	set_lru();

	// Open a file and read input line by line
	// load data into a structure called array
	// data will be divided into N and address
	addressPtr_t array = read_file(FILE_NAME, &size);

	for (int i = 0; i < size; i++)
	{
		// Assign address to data structure
		given_addr.addr_store = array[i].addr;
		// assign data from structure to specific variables
		N = array[i].n;
		index_sel = given_addr.bits.index; // index_sel will get the bits from 6-19.
		tag_sel = given_addr.bits.tag;	   // tag_sel will get the bits 20-31.

		// Check the value of N and proceed according to given parameters

		// check if hit or miss
		if (N == 0 || N == 1)
		{
			if (hit_or_miss(tag_sel, index_sel, N))
			{
				// increment hit value
				hits++;
				// update MESI bits and reads/writes
				cache_behaviour(N, index_sel, way_num, array[i].addr, mode);
				// update LRU bits
				UpdateLRUData(index_sel, way_num);
				// make note that this line was accessed
				data_cache[index_sel][way_num].line_accessed = 1;
			}
			else
			{
				// increment miss value
				misses++;
				// if mode 1 print relavent data
				if (mode == 1 && N == 0)
					printf("Read data from L2 <%x>\n", array[i].addr);
				if (mode == 1 && N == 1)
					printf("Read for Ownership from L2 <%x>\n", array[i].addr);
				// find invalid lines
				if (invalid_line(index_sel, N))
				{
					// update LRU bits
					UpdateLRUData(index_sel, way_num);
					// update tag
					data_cache[index_sel][way_num].tag_store = tag_sel;
					// update MESI bits and reads/writes
					cache_behaviour(N, index_sel, way_num, array[i].addr, mode);
					// make note that this line was accessed
					data_cache[index_sel][way_num].line_accessed = 1;
				}
				else // Evict the LRU cache
				{
					// find victim cache line
					way_num = victim_line(index_sel, N);
					// if mode 1 print relavent information
					if (mode == 1 && data_cache[index_sel][way_num].MESI == M)
						printf("write to L2 <%x>\n", array[i].addr);
					// update tag
					data_cache[index_sel][way_num].tag_store = tag_sel;
					// update LRU bits
					UpdateLRUData(index_sel, way_num);
					// update MESI bit
					data_cache[index_sel][way_num].MESI = I;
					// update MESI bits and reads/writes
					cache_behaviour(N, index_sel, way_num, array[i].addr, mode);
					// make note that this line was accessed
					data_cache[index_sel][way_num].line_accessed = 1;
				}
			}
		}
		// update MESI, tag, and lru for instruction cache
		else if (N == 2)
		{
			if (hit_or_miss(tag_sel, index_sel, N))
			{
				hits++;
				cache_behaviour(N, index_sel, way_num, array[i].addr, mode);
				UpdateLRUInstr(index_sel, way_num);
				instruction_cache[index_sel][way_num].line_accessed = 1;
			}
			else
			{

				if (mode == 1)
					printf("Read data from L2 <%x>\n", array[i].addr);
				misses++;
				if (invalid_line(index_sel, N))
				{
					UpdateLRUInstr(index_sel, way_num);
					instruction_cache[index_sel][way_num].tag_store = tag_sel;
					cache_behaviour(N, index_sel, way_num, array[i].addr, mode);
					instruction_cache[index_sel][way_num].line_accessed = 1;
				}
				else
				{
					way_num = victim_line(index_sel, N);
					if (mode == 1 && instruction_cache[index_sel][way_num].MESI == M)
						printf("write to L2 <%x>\n", array[i].addr);
					instruction_cache[index_sel][way_num].tag_store = tag_sel;
					UpdateLRUInstr(index_sel, way_num);
					instruction_cache[index_sel][way_num].MESI = I;
					cache_behaviour(N, index_sel, way_num, array[i].addr, mode);
					instruction_cache[index_sel][way_num].line_accessed = 1;
				}
			}
		}
		//L2 is involved
		else if (N == 3 || N == 4)
		{
			if (hit_or_miss(tag_sel, index_sel, N))
			{
				cache_behaviour(N, index_sel, way_num, array[i].addr, mode);
			}
		}
		// Clear the cache and reset all states
		else if (N == 8)
		{
			clear_reset();
		}
		// print contents of cache
		else if (N == 9)
		{
			print_accessed_lines();
		}
	}
	print_hit_miss();

	return 0;
}
