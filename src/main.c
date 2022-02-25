#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <"Defines.h">


int main()
{
	long N, address;
	input_addr given_addr;
        uint16_t index_sel, tag_sel;

        memset(instruction_cache, 0, CACHE_SIZE_INSTR);    // it will set our cache to all zeros to start with.
        memset(data_cache, 0, CACHE_SIZE_DATA);

         //assigning input address into the structure. this is done after we read data from the file and when we have got N and address separately.
	given_addr.addr_store = address;

	//after we have store our address in given_addr which is of type input_addr.  now we are extracting the desired bits we need.
	index_sel = given_addr.bits.index;                     //index_sel will get the bits from 6-19.
	tag_sel = given_addr.bits.tag;                               //tag_sel will get the bits 20-31.

	return 0;
}

