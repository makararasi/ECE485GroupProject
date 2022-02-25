#define SETS						16*1024                                                           //Same number of sets for both cache.
#define WAYS_DATA             		8                                                                        // ways for data cache
#define WAYS_INSTR           		4                                                                        // ways for instruction cache
#define BYTES						sizeof(stored_data)                                //these three extra lines are for our calculations.
#define CACHE_SIZE_DATA		(SETS * WAYS_DATA * BYTES)
#define CACHE_SIZE_INSTR		(SETS * WAYS_INSTR * BYTES)
#define LINE_LENGTH	                 250
#define FILE_NAME                               "exp.txt"

//MESI states:-
#define I					0
#define M					1
#define E					2
#define S					3

//Data structures:-
typedef union                                                                           //this union will have 32 bits memory space allocated to it
{
	struct __attribute__((__packed__))
	{
		uint8_t byte:6;
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

typedef struct address_s
{
    int n;
    long addr;
} address_t, *addressPtr_t;

//Global variables:-
int read_result = 0, write_result = 0, hits = 0, misses = 0, way_num;
stored_data instruction_cache[SETS][WAYS_INSTR];       //to store instruction cache line data which are of type stored_data (structure).
stored_data data_cache[SETS][WAYS_DATA];                       //to store data cache line .
uint8_t LRU_instruction[SETS][WAYS_INSTR];
uint8_t LRU_data[SETS][WAYS_DATA];

//Function definitions :-
bool valid_tag(uint8_t mesi);

bool hit_or_miss(uint16_t tag_select, uint16_t ip_index, uint8_t op_n );

int lru_invalid_line(uint16_t ip_index, uint8_t n_op);

bool invalid_line(uint16_t index, uint8_t n);

address_t *read_file(const char *filename, int *size)
