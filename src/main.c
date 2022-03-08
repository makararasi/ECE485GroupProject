/**************************************************************************************
 * File    : main.c
 * Project : ECE 485/585 L1 split cache.
 * Term    : Winter 2022
 * Authors : Luke Hoskam, Vemuri Kousalya Gayatri, Payal Kalamdar, Sasi Kiran Nalluri.
 **************************************************************************************/
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
		D printf("\nCurrent line number: %d\n", i);
		D printf("Current operation: %d\n", N);
		D printf("Current address: %x\n", array[i].addr);
		if (N == 0 || N == 1)
		{ // check if hit or miss
			D printf("N was either 0 or 1\n");
			if (hit_or_miss(tag_sel, index_sel, N))
			{
				D printf("We had a HIT!!\n");
				// increment hit value
				hits++;
				// update MESI bits and reads/writes
				cache_behaviour(N, index_sel, way_num);
				// update LRU bits
				UpdateLRUData(index_sel, way_num);
				// make note that this line was accessed
				data_cache[index_sel][way_num].line_accessed = 1;
			}
			else
			{
				D printf("We had a MISS!!\n");
				// increment miss value
				misses++;
				// if mode 1 print relavent data
				if (mode == 1 && N == 0)
				{
					D printf("N is 0\n");
					printf("Read data from L2 <%x>\n", array[i].addr);
				}
				if (mode == 1 && N == 1)
				{
					D printf("N is 1\n");
					printf("Read for Ownership from L2 <%x>\n", array[i].addr);
				}
				// find invalid lines
				if (invalid_line(index_sel, N))
				{
					D printf("Invalid line");
					// update LRU bits
					UpdateLRUData(index_sel, way_num);
					// update tag
					data_cache[index_sel][way_num].tag_store = tag_sel;
					// update MESI bits and reads/writes
					cache_behaviour(N, index_sel, way_num);
					// make note that this line was accessed
					data_cache[index_sel][way_num].line_accessed = 1;
				}
				else // Evict the LRU cache line.
				{
					D printf("We didn't have an invalid line... evicting\n");
					// find victim cache line
					way_num = victim_line(index_sel, N);
					// if mode 1 print relavent information
					if (mode == 1 && data_cache[index_sel][way_num].MESI == M)
					{
						D printf("ONLY PRINTING IF MODE IS 1\n");
						printf("Write to L2 <%x>\n", array[i].addr);
					}
					// update tag
					data_cache[index_sel][way_num].tag_store = tag_sel;
					// update LRU bits
					UpdateLRUData(index_sel, way_num);
					// update previous MESI bits to I.
					data_cache[index_sel][way_num].MESI = I;
					// update MESI bits and reads/writes
					cache_behaviour(N, index_sel, way_num);
					// make note that this line was accessed
					data_cache[index_sel][way_num].line_accessed = 1;
				}
			}
		}
		// update MESI, tag, and lru for instruction cache
		else if (N == 2)
		{
			D printf("N is 2\n");
			if (hit_or_miss(tag_sel, index_sel, N))
			{
				D printf("We had a HIT!!\n");
				hits++;
				cache_behaviour(N, index_sel, way_num);
				UpdateLRUInstr(index_sel, way_num);
				instruction_cache[index_sel][way_num].line_accessed = 1;
			}
			else
			{
				D printf("We had a MISS!!\n");
				if (mode == 1)
				{

					printf("Read data from L2 <%x>\n", array[i].addr);
				}
				misses++;
				if (invalid_line(index_sel, N))
				{
					D printf("Invalid line!\n");
					UpdateLRUInstr(index_sel, way_num);
					instruction_cache[index_sel][way_num].tag_store = tag_sel;
					cache_behaviour(N, index_sel, way_num);
					instruction_cache[index_sel][way_num].line_accessed = 1;
				}
				else
				{
					D printf("Line is valid Evicting!\n");
					way_num = victim_line(index_sel, N);
					if (mode == 1 && instruction_cache[index_sel][way_num].MESI == M)
					{
						printf("Write to L2 <%x>\n", array[i].addr);
					}
					instruction_cache[index_sel][way_num].tag_store = tag_sel;
					UpdateLRUInstr(index_sel, way_num);
					instruction_cache[index_sel][way_num].MESI = I;
					cache_behaviour(N, index_sel, way_num);
					instruction_cache[index_sel][way_num].line_accessed = 1;
				}
			}
		}
		// Behavior according to snoop
		else if (N == 3 || N == 4)
		{
			D printf("N is 3 or 4\n");
			if (hit_or_miss(tag_sel, index_sel, N))
			{
				D printf("We had a HIT!!\n");
				if (N == 4 && mode == 1 && data_cache[index_sel][way_num].MESI == M)
				{
					D printf("N is 4 and MODE is 1\n");
					printf("Return data to L2 <%x>\n", array[i].addr);
				}
				cache_behaviour(N, index_sel, way_num);
				UpdateLRUData(index_sel, way_num);
			}
		}
		// Clear the cache and reset all states
		else if (N == 8)
		{
			D printf("N is 8\n");
			clear_reset();
		}
		// print contents of cache lines which were accessed.
		else if (N == 9)
		{
			printf("N is 9\n");
			print_accessed_lines();
		}

		D printf("\n--------------------------------------------\n--------------------------------------------\n");
	}
	print_hit_miss();

	return 0;
}