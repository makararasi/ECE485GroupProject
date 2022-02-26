#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "include/Defines.h"
#include <inttypes.h>

int main()
{
	int N;
	input_addr given_addr;
	uint16_t index_sel, tag_sel;

	memset(instruction_cache, 0, CACHE_SIZE_INSTR); // it will set our cache to all zeros to start with.
	memset(data_cache, 0, CACHE_SIZE_DATA);

	int size; // Size of array read
	addressPtr_t array = read_file(FILE_NAME, &size);
	// assigning input address into the structure. this is done after we read data from the file and when we have got N and address separately.
	// given_addr.addr_store = address;
	for (int i = 0; i < size; i++)
	{

		given_addr.addr_store = array[i].addr;

		N = array[i].n;
		printf("\nN: %d\n", N);
		index_sel = given_addr.bits.index; // index_sel will get the bits from 6-19.
		tag_sel = given_addr.bits.tag;	   // tag_sel will get the bits 20-31.
		if (N == 0 || N == 1)
		{
			if (hit_or_miss(tag_sel, index_sel, N))
			{
				printf("HIT\n");
				hits++;
				cache_behaviour(N, index_sel, way_num);
				UpdateLRUData(index_sel, way_num);
				data_cache[index_sel][way_num].line_accessed = 1;
			}
			else
			{
				printf("MISS\n");
				misses++;
				if (invalid_line(index_sel, N))
				{
					UpdateLRUData(index_sel, way_num);
					data_cache[index_sel][way_num].tag_store = tag_sel;
					cache_behaviour(N, index_sel, way_num);
					data_cache[index_sel][way_num].line_accessed = 1;
				}
				else
				{
					way_num = victim_line(index_sel, N);
					data_cache[index_sel][way_num].tag_store = tag_sel;
					UpdateLRUData(index_sel, way_num);
					data_cache[index_sel][way_num].MESI = I;
					cache_behaviour(N, index_sel, way_num);
					data_cache[index_sel][way_num].line_accessed = 1;
				}
			}
		}
		else if(N == 2) {

			if (hit_or_miss(tag_sel, index_sel, N))
			{
				printf("HIT\n");
				hits++;
				cache_behaviour(N, index_sel, way_num);
				UpdateLRUInstr(index_sel, way_num);
				instruction_cache[index_sel][way_num].line_accessed = 1;
			}
			else
			{
				printf("miss\n");
				misses++;
				if (invalid_line(index_sel, N))
				{
					UpdateLRUInstr(index_sel, way_num);
					instruction_cache[index_sel][way_num].tag_store = tag_sel;
					cache_behaviour(N, index_sel, way_num);
					instruction_cache[index_sel][way_num].line_accessed = 1;
				}
				else
				{
					way_num = victim_line(index_sel, N);
					instruction_cache[index_sel][way_num].tag_store = tag_sel;
					UpdateLRUInstr(index_sel, way_num);
					instruction_cache[index_sel][way_num].MESI = I;
					cache_behaviour(N, index_sel, way_num);
					instruction_cache[index_sel][way_num].line_accessed = 1;
				}
			}
			
			
		}
		else if(N == 3 || N == 4) {
			if(hit_or_miss(tag_sel, index_sel, N)) {
				cache_behaviour(N, index_sel, way_num);
			}
			
		}
		else if( N == 8){
			clear_reset();
		}
		else if(N == 9) {
			print_accessed_lines();
		}
		printf("i: %d\n\n",i);
	}
	print_hit_miss();

	return 0;
}
