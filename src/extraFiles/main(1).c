#include <stdio.h>
#include <string.h>

#define SETS						16*1024                                                           //Same number of sets for both cache.
#define WAYS_DATA             		8                                                                        // ways for data cache
#define WAYS_INSTR           		4                                                                        // ways for instruction cache
#define BYTES						sizeof(stored_data)                                //these three extra lines are for our calculations.
#define CACHE_SIZE_DATA		(SETS * WAYS_DATA * BYTES)
#define CACHE_SIZE_INSTR		(SETS * WAYS_INSTR * BYTES)

typedef union                                                                           //this union will have 32 bits memory space allocated to it
{
	struct __attribute__((__packed__))
	{
		uint8_t snoop:2;
		uint8_t byte:4;
		uint16_t index:14;
		uint16_t tag:12;
	} bits;
	uint32_t addr_store;            // we will store the retrived address from the file and will access them by the size we need (as given in the above structure).
} input_addr;

typedef struct __attribute__((__packed__))             //this structure will have 16 bits allocated to it.
{
	uint16_t tag_store:12;
	uint8_t MESI:2;
	uint8_t reserved:2;
} stored_data;

stored_data instruction_cache[SETS][WAYS_INSTR];       //to store instruction cache line data which are of type stored_data (structure).
stored_data data_cache[SETS][WAYS_DATA];                       //to store data cache line .

int main()
{
	long N, address;
	input_addr given_addr;
        uint8_t snoop_sel;
        uint16_t index_sel, tag_sel;

        memset(instruction_cache, 0, CACHE_SIZE_INSTR);    // it will set our cache to all zeros to start with.
        memset(data_cache, 0, CACHE_SIZE_DATA);

        //we can start our file open from here
        //once we get our N (traces operation) we can atore it in N variable to select which operation we need to do.
        //after N we will get our address which will be store in the format given bellow.

        //assigning input address into the structure. this is done after we read data from the file and when we have got N and address separately.
	given_addr.addr_store = address;

	//after we have store our address in given_addr which is of type input_addr.  now we are extracting the desired bits we need.
	snoop_sel = given_addr.bits.snoop;                    //snoop_sel will get the bits LSB 0 and 1 position.
	index_sel = given_addr.bits.index;                     //index_sel will get the bits from 6-19.
	tag_sel = given_addr.bits.tag;                               //tag_sel will get the bits 20-31.

	return 0;
}


