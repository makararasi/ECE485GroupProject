#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "Defines.h"
#include <inttypes.h>

int main()
{
	int N,mode;
    int size; // Size to read
	input_addr given_addr;
	uint16_t index_sel, tag_sel;

    printf("Enter the desired mode :- ");
    scanf("%d",&mode);
    printf("the desired mode is :- %d\n",mode);
    
	memset(instruction_cache, 0, CACHE_SIZE_INSTR); // it will set our cache to all zeros to start with.
	memset(data_cache, 0, CACHE_SIZE_DATA);
    set_lru ();
    
	addressPtr_t array = read_file(FILE_NAME, &size);

	for (int i = 0; i < size; i++)
	{

		given_addr.addr_store = array[i].addr;

		N = array[i].n;
		index_sel = given_addr.bits.index; // index_sel will get the bits from 6-19.
		tag_sel = given_addr.bits.tag;	   // tag_sel will get the bits 20-31.
		if (N == 0 || N == 1)
		{
			if (hit_or_miss(tag_sel, index_sel, N))
			{
				hits++;
				cache_behaviour(N, index_sel, way_num,array[i].addr,mode);
				UpdateLRUData(index_sel, way_num);
				data_cache[index_sel][way_num].line_accessed = 1;
			}
			else
			{
				misses++;
                if(mode == 1 && N == 0){
                        printf("Read data from L2 <%x>\n",array[i].addr);
                }
                if(mode == 1 && N == 1){
                        printf("Read for Ownership from L2 <%x>\n",array[i].addr);
                }

				if (invalid_line(index_sel, N))
				{
					UpdateLRUData(index_sel, way_num);
					data_cache[index_sel][way_num].tag_store = tag_sel;
					cache_behaviour(N, index_sel, way_num,array[i].addr,mode);
					data_cache[index_sel][way_num].line_accessed = 1;
				}
				else
				{
					way_num = victim_line(index_sel, N);
                    if(mode == 1 && data_cache[index_sel][way_num].MESI == M){
                        printf("write to L2 <%x>\n",array[i].addr);
                    }
					data_cache[index_sel][way_num].tag_store = tag_sel;
					UpdateLRUData(index_sel, way_num);
					data_cache[index_sel][way_num].MESI = I;
					cache_behaviour(N, index_sel, way_num,array[i].addr,mode);
					data_cache[index_sel][way_num].line_accessed = 1;
				}
			}
		}
		else if(N == 2) {

			if (hit_or_miss(tag_sel, index_sel, N))
			{
				//printf("HIT\n");
				hits++;
				cache_behaviour(N, index_sel, way_num,array[i].addr,mode);
				UpdateLRUInstr(index_sel, way_num);
				instruction_cache[index_sel][way_num].line_accessed = 1;
			}
			else
			{
				//printf("miss\n");
                if(mode == 1){
                    printf("Read data from L2 <%x>\n",array[i].addr);
                }
				misses++;
				if (invalid_line(index_sel, N))
				{
					UpdateLRUInstr(index_sel, way_num);
					instruction_cache[index_sel][way_num].tag_store = tag_sel;
					cache_behaviour(N, index_sel, way_num,array[i].addr,mode);
					instruction_cache[index_sel][way_num].line_accessed = 1;
				}
				else
				{
					way_num = victim_line(index_sel, N);
                    if(mode == 1 && instruction_cache[index_sel][way_num].MESI == M){
                        printf("write to L2 <%x>\n",array[i].addr);
                    }
					instruction_cache[index_sel][way_num].tag_store = tag_sel;
					UpdateLRUInstr(index_sel, way_num);
					instruction_cache[index_sel][way_num].MESI = I;
					cache_behaviour(N, index_sel, way_num,array[i].addr,mode);
					instruction_cache[index_sel][way_num].line_accessed = 1;
				}
			}
			
			
		}
		else if(N == 3 || N == 4) {
			if(hit_or_miss(tag_sel, index_sel, N)) {
				cache_behaviour(N, index_sel, way_num,array[i].addr,mode);
			}
			
		}
		else if( N == 8){
			clear_reset();
		}
		else if(N == 9) {
			print_accessed_lines();
		}
	}
	print_hit_miss();

	return 0;
}
