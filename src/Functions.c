#include <"Defines.h">

bool valid_tag(uint8_t mesi)
{
	if (mesi == M || mesi == E || mesi == S )
		return true;
	return false;
}

bool hit_or_miss(uint16_t tag_select, uint16_t ip_index, uint8_t op_n )
{
	uint16_t tagstored;
	for (int i = 0; i < WAYS_DATA; i++)
	{
		if(op_n == 0 || op_n == 1 || op_n == 3 || op_n == 4)
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
			if(i < WAYS_INSTR)
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

int lru_invalid_line(uint16_t ip_index, uint8_t n_op)
{
	int least = way_num, mesi;
	for(int i = 0; i < way_num; i++)
	{
		if(n_op == 0 || n_op == 1)
		{
			mesi = data_cache[index][i].MESI;
			if((mesi == I) && (LRU_data[ip_index][i] < LRU_data[ip_index][least]))
				least = i;
		}
		else
		{
			mesi = instruction_cache[index][i].MESI;
			if((mesi == I) && (LRU_instruction[ip_index][i] < LRU_instruction[ip_index][least]))
				least = i;
		}
	}
	return least;
}

bool invalid_line(uint16_t index, uint8_t n)
{
	uint8_t mesi_state, count = 0;
	if(n == 0 || n == 1)
	{
		for(int i = 0; i < WAYS_DATA; i++)
		{
			mesi_state = data_cache[index][i].MESI;
			if(mesi_state == I)
			{     count++;
				way_num = i;
			}
		}
	}
	else
	{
		for(int j = 0; j < WAYS_INSTR; j++)
		{
			mesi_state = instruction_cache[index][j].MESI;
			if(mesi_state == I)
			{     count++;
				way_num = j;
			}
		}
	}

	if(count == 1)
		return true;
	else if(count > 1)
	{
		way_num = lru_invalid_line(index, n);
		return true;
	}
	else
		return false;
}

address_t *read_file(const char *filename, int *size)
{
    FILE *fp;          // declare filepointer
    char *line = NULL; // input line from file variable
    size_t len = 0;
    ssize_t read;        // number of characters read on the line
    char *ptr;           // this isn't really used. But holds chars that aren't in the number
    address_t *number = NULL; // the array of addresses read
    int numElements = 0; // size of the array

    fp = fopen(FILENAME, "r");
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
        strcpy(temp2,line+2);
        
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


// from: https://stackoverflow.com/questions/8257714/how-to-convert-an-int-to-string-in-c
/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
 */
char *itoa(int value, char *result, int base)
{
    // check that the base if valid
    if (base < 2 || base > 36)
    {
        *result = '\0';
        return result;
    }

    char *ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do
    {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + (tmp_value - value * base)];
    } while (value);

    // Apply negative sign
    if (tmp_value < 0)
        *ptr++ = '-';
    *ptr-- = '\0';
    while (ptr1 < ptr)
    {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}


