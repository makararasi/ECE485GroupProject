/**************************************************************************************
 * File    : Defines.c
 * Project : ECE 485/585 L1 split cache.
 * Term    : Winter 2022
 * Authors : Luke Hoskam, Vemuri Kousalya Gayatri, Payal Kalamdar, Sasi Kiran Nalluri.
 **************************************************************************************/
 
#include "Defines.h"
int read_result = 0;
int write_result = 0;
int hits = 0;
int misses = 0;
int way_num;
//int mode;

/*******************************************************************************
* Checks if the tag selected is a valid line or not.
* Inputs: mesi - the lines MESI bits to check the state.
* Output: Return true is states are in M,E or S or returns false if in I state.
********************************************************************************/
bool valid_tag(uint8_t mesi)
{
	if (mesi == M || mesi == E || mesi == S){
		return true;
	}
	return false;
}

/****************************************************************************************************
* Checks if the tag selected is a hit or a miss.
* Inputs: tag_select (comes from the address), ip_index (comes from the address), op_n (operation N).
* Output: Returns true is a Hit, or returns false if miss.
*****************************************************************************************************/
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
				if (valid_tag(data_cache[ip_index][i].MESI)){
					return true;
				}
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
					if (valid_tag(instruction_cache[ip_index][i].MESI)){
						return true;
					}
					return false;
				}
			}
		}
	}
	return false;
}

/**************************************************************************************************************
* Searchs for an Invalid line in a set which is Least Recently Used if more than one invalid lines are present. 
* Inputs: ip_index (comes from the address), n_op (operation N from trace file).
* Output: Returns the way number which is an invalid line and LRU.
***************************************************************************************************************/
int lru_invalid_line(uint16_t ip_index, uint8_t n_op)
{
	int least = way_num, mesi;
	for (int i = 0; i < way_num; i++)
	{
		if (n_op == 0 || n_op == 1)
		{
			mesi = data_cache[ip_index][i].MESI;
			if ((mesi == I) && (LRU_data[ip_index][i] < LRU_data[ip_index][least])){
				least = i;
			}
		}
		else
		{
			mesi = instruction_cache[ip_index][i].MESI;
			if ((mesi == I) && (LRU_instruction[ip_index][i] < LRU_instruction[ip_index][least])){
				least = i;
			}
		}
	}
	return least;
}

/****************************************************************************************
* Checks if the set contains an invalid line or not.
* Inputs: index (comes from the address), n (operation N from trace file).
* Output: Returns true if a line is in I state, or returns false if there is no such line.
*****************************************************************************************/
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
	else{
		return false;
	}
}

/***********************************************
* Sets the LRU bits to start the counter method. 
* Inputs: None
* Output: None
************************************************/
void set_lru ()
{
    for(int i=0; i<SETS; i++)
    {
        for(int j=0; j<WAYS_DATA; j++)
        {
            LRU_data[i][j]=j;
        }
    }
    for(int k=0; k<SETS;k++)
    {
        for(int l=0;l<WAYS_INSTR;l++)
        {
            LRU_instruction[k][l]=l;
        }
    }
}

/**********************************************************
* Updates LRU bits whenever we access a line in Data cache.
* Inputs: None
* Output: None
***********************************************************/
void UpdateLRUData(uint16_t index,int way)
{
   uint8_t lru = LRU_data[index][way];
   for (int i=0; i<WAYS_DATA; i++)
   {
       if(LRU_data[index][i]>lru){
        LRU_data[index][i]--;
       }
   }
   LRU_data[index][way]=MRU_DATA;
}

/*****************************************************************
* Updates LRU bits whenever we access a line in Instruction cache.
* Inputs: None
* Output: None
******************************************************************/
void UpdateLRUInstr(uint16_t index, int way)
{
    uint16_t lru = LRU_instruction[index][way];
    for(int j=0; j<WAYS_INSTR;j++)
    {
        if(LRU_instruction[index][j]>lru){
            LRU_instruction[index][j]--;
        }
    }
    LRU_instruction[index][way]=MRU_INSTR;
}

/**************************************************************************************
* Clears all the contents in both the caches, resets MESI state to I and sets LRU bits. 
* Inputs: None
* Output: None
***************************************************************************************/
void clear_reset(void)
{
	memset(instruction_cache, 0, CACHE_SIZE_INSTR);
    memset(data_cache, 0, CACHE_SIZE_DATA);
    set_lru( );
}


/******************************************************
 * Reads a file of addresses and returns an array of addresses
 * 
 * Inputs: filename pointer to the name of the file to be read
 * Inputs: size the size of the array
 * Output: An array of addresses.
*******************************************************/
address_t *read_file(const char *filename, int *size)
{
	FILE *fp;		   
	char *line = NULL; 
	size_t len = 0;
	ssize_t read;			  
	char *ptr;				  
	address_t *number = NULL; 
	int numElements = 0;	  

	fp = fopen(filename, "r");
	if (fp == NULL){
		exit(EXIT_FAILURE);
	}

	while ((read = getline(&line, &len, fp)) != -1)
	{

		numElements++;
		number = realloc(number, numElements * sizeof(address_t));
		char temp[2];
		char temp2[10];
		strncpy(temp, line, 2);

		number[numElements - 1].n = atoi(temp);
		int j = 0;
		for(int i = 2; i<10; i++){
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

/******************************************************
 * If N = 0, read_result is incremented.
 * If N = 1, write_result is incremented.
 * If N = 2, read_result is incremented.
 * If N = 3, data_cache[index][way_num].MESI is set to I.
 * If N = 4, data_cache[index][way_num].MESI is set to I
 * 
 * Inputs: N 0 for read, 1 for write, 2 for instruction fetch
 * Inputs: index the index of the cache block to be accessed (from 0 to N-1)
 * Inputs: way_num the number of the way in the set
 * Inputs: addr the address of the cache line we want to access
 * Inputs: mode 0 for data, 1 for instruction
 * Output: None
*******************************************************/
void cache_behaviour(int N, uint16_t index, int way_num, uint32_t addr,int mode)
{
    
	if (N == 0)
	{
		read_result++;
		if (data_cache[index][way_num].MESI == I){
			data_cache[index][way_num].MESI = E; }
		else if (data_cache[index][way_num].MESI == M){
			data_cache[index][way_num].MESI = M; }
		else if (data_cache[index][way_num].MESI == E){
			data_cache[index][way_num].MESI = S; }
	}
	else if (N == 1)
	{
		write_result++;
		data_cache[index][way_num].MESI = M;
	}
	else if (N == 2)
	{
		read_result++;
		if (instruction_cache[index][way_num].MESI == I){
			instruction_cache[index][way_num].MESI = E; }
		else if (instruction_cache[index][way_num].MESI == M){
			instruction_cache[index][way_num].MESI = M; }
		else if (instruction_cache[index][way_num].MESI == E){
			instruction_cache[index][way_num].MESI = S;}
	}
	else if (N == 3 || N == 4)
	{   
        if(N == 4 && mode == 1 && data_cache[index][way_num].MESI == M){
            printf("Read data to L2 <%x>\n",addr);
        }
		data_cache[index][way_num].MESI = I;
	}
}


/******************************************************
 * Prints out the number of hits, misses, reads, and writes
 * Inputs: None
 * Output: None
*******************************************************/
void print_hit_miss(void)
{
	printf("----------------------------------------------------\n");
	printf("----------------------------------------------------\n");
	printf("Number of hits  : %d\n", hits);
	printf("Number of misses: %d\n", misses);
	printf("Number of reads : %d\n", read_result);
	printf("Number of writes: %d\n", write_result);

	float avg_hit = (float)(hits) / (float)(hits + misses);
	printf("Cache hit ratio: %.4f = %.2f %%\n", avg_hit, (avg_hit * 100));
	printf("----------------------------------------------------\n");
	printf("----------------------------------------------------\n");
}

/******************************************************
* Prints the Data of cache lines which wwhere accessed. 
* Inputs: None
* Output: None
*******************************************************/
void print_accessed_lines(void)
{
	char state;
	uint8_t lru;
	uint16_t tag;
	printf("Accessed lines of DATA CACHE : \n");
	printf("Index\tWays\tState\tLRU\tTag\n");
	for(int i = 0; i < SETS; i++)
	{	for(int j = 0; j < WAYS_DATA; j++)
		{	if (data_cache[i][j].MESI == I){
				state = 'I';}
			else if (data_cache [i][j].MESI == M){
				state = 'M';}
			else if (data_cache [i][j].MESI == E){
				state = 'E';}
			else if (data_cache [i][j].MESI == S){
				state = 'S';}
			tag = data_cache [i][j].tag_store;
			lru = LRU_data[i][j];
			if (data_cache[i][j].line_accessed){
				printf ("%x\t%d\t%c\t%x\t%x\n", i, j, state, lru, tag);
			}
		}
	}
    printf("\n");
	printf("Accessed lines of INSTRUCTION CACHE : \n");
	printf("Index\tWays\tState\tLRU\tTag\n");
	for(int i = 0; i < SETS; i++)
	{	for(int j = 0; j < WAYS_INSTR; j++)
		{	if (instruction_cache[i][j].MESI == I){
				state = 'I';}
			else if (instruction_cache[i][j].MESI == M){
				state = 'M';}
			else if (instruction_cache[i][j].MESI == E){
				state = 'E';}
			else if (instruction_cache[i][j].MESI == S){
				state = 'S';}
			tag = instruction_cache[i][j].tag_store;
			lru = LRU_instruction[i][j];
			if (instruction_cache[i][j].line_accessed){
				printf ("%x\t%d\t%c\t%x\t%x\n", i, j, state, lru, tag);
			}
		}
	}
    printf("\n");
}


/*************************************************************************
* Searchs the Least Recently Used line in the given set/index. 
* Inputs: index (comes from the address), n (operation N from trace file).
* Output: Returns the way at which the LRU line is found. 
**************************************************************************/
int victim_line(uint16_t index, uint8_t n){

	if(n == 1 || n == 0) {
		for(int i = 0; i < WAYS_DATA; i++) {
			if(LRU_data[index][i] == LRU){ 
				return i;}
		}
	}
	else if(n == 2) {
		for(int i = 0; i < WAYS_DATA; i++) {
			if(LRU_instruction[index][i] == LRU){ 
				return i;}
		}
	}
	return 0;
}
