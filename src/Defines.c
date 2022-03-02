#include "Defines.h"
int read_result = 0;
int write_result = 0;
int hits = 0;
int misses = 0;
int way_num;

/**
 * If the tag is M, E, or S, return true. Otherwise, return false
 *
 * @param mesi the MESI protocol state of the cache line
 *
 * @return Nothing.
 */
bool valid_tag(uint8_t mesi)
{
	if (mesi == M || mesi == E || mesi == S)
		return true;
	return false;
}

/**
 * If the tag stored in the cache is equal to the tag selected, then the cache line is hit
 *
 * @param tag_select The tag that we are looking for in the cache.
 * @param ip_index The index of the cache block we're looking at.
 * @param op_n 0 = read, 1 = read-modify-write, 2 = write, 3 = write-through, 4 = write-back
 *
 * @return The way number of the cache line that was hit or miss.
 */
bool hit_or_miss(uint16_t tag_select, uint16_t ip_index, uint8_t op_n)
{
	uint16_t tagstored;
	for (int i = 0; i < WAYS_DATA; i++)
	{
		if (op_n == 0 || op_n == 1 || op_n == 3 || op_n == 4)
		{
			tagstored = data_cache[ip_index][i].tag_store;
			if (tagstored == tag_select)
			{
				way_num = i;
				if (valid_tag(data_cache[ip_index][i].MESI))
					return true;
				return false;
			}
		}
		else
		{
			if (i < WAYS_INSTR)
			{
				tagstored = instruction_cache[ip_index][i].tag_store;
				if (tagstored == tag_select)
				{
					way_num = i;
					if (valid_tag(instruction_cache[ip_index][i].MESI))
						return true;
					return false;
				}
			}
		}
	}
	return false;
}

/**
 * If there is a single invalid line in the cache, return true. If there are multiple invalid lines,
 * return true and set the way_num variable to the index of the least recently used line
 *
 * @param ip_index The index of the cache line to be invalidated.
 * @param n_op 0 for data, 1 for instruction
 *
 * @return The way number of the line that is being invalidated.
 */
int lru_invalid_line(uint16_t ip_index, uint8_t n_op)
{
	int least = way_num, mesi;
	for (int i = 0; i < way_num; i++)
	{
		if (n_op == 0 || n_op == 1)
		{
			mesi = data_cache[ip_index][i].MESI;
			if ((mesi == I) && (LRU_data[ip_index][i] < LRU_data[ip_index][least]))
				least = i;
		}
		else
		{
			mesi = instruction_cache[ip_index][i].MESI;
			if ((mesi == I) && (LRU_instruction[ip_index][i] < LRU_instruction[ip_index][least]))
				least = i;
		}
	}
	return least;
}

/**
 * If there is only one line in the set that is in the invalid state, then return true. Otherwise,
 * return false
 *
 * @param index The index of the cache block to be invalidated.
 * @param n 0 for data, 1 for instruction
 *
 * @return The way number of the line that is being invalidated.
 */
bool invalid_line(uint16_t index, uint8_t n)
{
	uint8_t mesi_state, count = 0;
	if (n == 0 || n == 1)
	{
		for (int i = 0; i < WAYS_DATA; i++)
		{
			mesi_state = data_cache[index][i].MESI;
			if (mesi_state == I)
			{
				count++;
				way_num = i;
			}
		}
	}
	else
	{
		for (int j = 0; j < WAYS_INSTR; j++)
		{
			mesi_state = instruction_cache[index][j].MESI;
			if (mesi_state == I)
			{
				count++;
				way_num = j;
			}
		}
	}

	if (count == 1)
		return true;
	else if (count > 1)
	{
		way_num = lru_invalid_line(index, n);
		return true;
	}
	else
		return false;
}

/**
 * This function sets the LRU values of the data and instruction cache to
 * 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 respectively
 */
void set_lru()
{
	for (int i = 0; i < SETS; i++)
	{
		for (int j = 0; j < WAYS_DATA; j++)
		{
			LRU_data[i][j] = j;
		}
	}
	for (int k = 0; k < SETS; k++)
	{
		for (int l = 0; l < WAYS_INSTR; l++)
		{
			LRU_instruction[k][l] = l;
		}
	}
}

/**
 * UpdateLRUData(index,way)
 *
 * This function updates the LRU_data array for the given index and way
 *
 * @param index The index of the cache block we're updating.
 * @param way the way of the cache block
 */
void UpdateLRUData(uint16_t index, int way)
{
	uint8_t lru = LRU_data[index][way];
	for (int i = 0; i < WAYS_DATA; i++)
	{
		if (LRU_data[index][i] > lru)
			LRU_data[index][i]--;
	}
	LRU_data[index][way] = MRU_DATA;
}

/**
 * UpdateLRUInstr() updates the LRU bits for the instruction cache
 *
 * @param index The index of the cache block we're updating.
 * @param way the way of the cache block we want to update
 */
void UpdateLRUInstr(uint16_t index, int way)
{
	uint16_t lru = LRU_instruction[index][way];
	for (int j = 0; j < WAYS_INSTR; j++)
	{
		if (LRU_instruction[index][j] > lru)
			LRU_instruction[index][j]--;
	}
	LRU_instruction[index][way] = MRU_INSTR;
}

/**
 * Clear the instruction and data cache, and reset the LRU bits
 */
void clear_reset(void)
{
	memset(instruction_cache, 0, CACHE_SIZE_INSTR);
	memset(data_cache, 0, CACHE_SIZE_DATA);
	set_lru();
}

/**
 * Reads a file of addresses and stores them in an array
 *
 * @param filename the name of the file to read from
 * @param size the size of the array
 *
 * @return An array of addresses.
 */
address_t *read_file(const char *filename, int *size)
{
	FILE *fp;		   // declare filepointer
	char *line = NULL; // input line from file variable
	size_t len = 0;
	ssize_t read;			  // number of characters read on the line
	char *ptr;				  // this isn't really used. But holds chars that aren't in the number
	address_t *number = NULL; // the array of addresses read
	int numElements = 0;	  // size of the array

	fp = fopen(filename, "r");
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
		// strcpy(temp2, line + 2);
		int j = 0;
		for (int i = 2; i < 10; i++)
		{
			temp2[j++] = line[i];
		}

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

/**
 * The cache_behaviour function is used to update the MESI state of the cache line
 *
 * @param N 0 for read, 1 for write, 2 for instruction fetch
 * @param index The index of the cache block to be accessed.
 * @param way_num the number of the way in the set
 * @param addr The address of the cache line that is being accessed.
 * @param mode 0 for data, 1 for instruction
 */
void cache_behaviour(int N, uint16_t index, int way_num, uint32_t addr, int mode)
{

	if (N == 0)
	{
		read_result++;
		if (data_cache[index][way_num].MESI == I)
			data_cache[index][way_num].MESI = E;
		else if (data_cache[index][way_num].MESI == M)
			data_cache[index][way_num].MESI = M;
		else if (data_cache[index][way_num].MESI == E)
			data_cache[index][way_num].MESI = S;
		else if (data_cache[index][way_num].MESI == S)
			data_cache[index][way_num].MESI == S;
	}
	else if (N == 1)
	{
		write_result++;
		if (data_cache[index][way_num].MESI == I)
			data_cache[index][way_num].MESI = M;
		else if (data_cache[index][way_num].MESI == M)
			data_cache[index][way_num].MESI = M;
		else if (data_cache[index][way_num].MESI == E)
			data_cache[index][way_num].MESI = M;
		else if (data_cache[index][way_num].MESI == S)
			data_cache[index][way_num].MESI == M;
	}
	else if (N == 2)
	{
		read_result++;
		if (instruction_cache[index][way_num].MESI == I)
			instruction_cache[index][way_num].MESI = E;
		else if (instruction_cache[index][way_num].MESI == M)
			instruction_cache[index][way_num].MESI = M;
		else if (instruction_cache[index][way_num].MESI == E)
			instruction_cache[index][way_num].MESI = S;
		else if (instruction_cache[index][way_num].MESI == S)
			instruction_cache[index][way_num].MESI == S;
	}
	else if (N == 3 || N == 4)
	{
		if (N == 4 && mode == 1 && data_cache[index][way_num].MESI == M)
			printf("Read data to L2 <%x>\n", addr);
		data_cache[index][way_num].MESI = I;
	}
}

/**
 * Prints out the number of hits, misses, reads, and writes
 */
void print_hit_miss(void)
{
	printf("----------------------------------------------------\n");
	printf("----------------------------------------------------\n");
	printf("Number of hits: %d\n", hits);
	printf("Number of misses: %d\n", misses);
	printf("Number of reads: %d\n", read_result);
	printf("Number of writes: %d\n", write_result);

	float avg_hit = (float)(hits) / (float)(hits + misses);
	printf("Cache hit ratio: %.4f = %.2f %%\n", avg_hit, (avg_hit * 100));
	printf("----------------------------------------------------\n");
	printf("----------------------------------------------------\n");
}

/**
 * Prints the state of all the lines in the data and instruction cache
 */
void print_accessed_lines(void)
{
	char state;
	uint8_t lru;
	uint16_t tag;
	printf("Accessed lines of DATA CACHE : \n");
	printf("Index\tWays\tState\tLRU\tTag\n");
	for (int i = 0; i < SETS; i++)
	{
		for (int j = 0; j < WAYS_DATA; j++)
		{
			if (data_cache[i][j].MESI == I)
				state = 'I';
			else if (data_cache[i][j].MESI == M)
				state = 'M';
			else if (data_cache[i][j].MESI == E)
				state = 'E';
			else if (data_cache[i][j].MESI == S)
				state = 'S';
			tag = data_cache[i][j].tag_store;
			lru = LRU_data[i][j];
			if (data_cache[i][j].line_accessed)
				printf("%x\t%d\t%c\t%x\t%x\n", i, j, state, lru, tag);
		}
	}
	printf("\n");
	printf("Accessed lines of INSTRUCTION CACHE : \n");
	printf("Index\tWays\tState\tLRU\tTag\n");
	for (int i = 0; i < SETS; i++)
	{
		for (int j = 0; j < WAYS_INSTR; j++)
		{
			if (instruction_cache[i][j].MESI == I)
				state = 'I';
			else if (instruction_cache[i][j].MESI == M)
				state = 'M';
			else if (instruction_cache[i][j].MESI == E)
				state = 'E';
			else if (instruction_cache[i][j].MESI == S)
				state = 'S';
			tag = instruction_cache[i][j].tag_store;
			lru = LRU_instruction[i][j];
			if (instruction_cache[i][j].line_accessed)
				printf("%x\t%d\t%c\t%x\t%x\n", i, j, state, lru, tag);
		}
	}
	printf("\n");
}

/**
 * Given a cache index and a number of ways, return the index of the least recently used way
 *
 * @param index the index of the cache block we're looking at
 * @param n 1 for data, 2 for instruction
 *
 * @return The index of the line that is being replaced.
 */
int victim_line(uint16_t index, uint8_t n)
{

	if (n == 1 || n == 0)
	{
		for (int i = 0; i < WAYS_DATA; i++)
		{
			if (LRU_data[index][i] == LRU)
				return i;
		}
	}
	else if (n == 2)
	{
		for (int i = 0; i < WAYS_DATA; i++)
		{
			if (LRU_instruction[index][i] == LRU)
				return i;
		}
	}
	return 0;
}
