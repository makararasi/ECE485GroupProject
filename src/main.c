#include <stdio.h>

const char *readSTDIN(int *count);

int main()
{

}

const char *readSTDIN(int *count)
{
	int n, i;
	char buf[BUFFSIZE];
	char *inputElements = malloc(sizeof(char));
	int stringElements = 0;
	int elementCount = 0;

	while ((n = read(STDIN_FILENO, buf, BUFFSIZE)) > 0) // Read from STDIN and set the number read to n
	{
		if (n < 0)
		{
			perror("read error");
		}
		for (i = 0; i < n; i++)
		{
			if (isspace(buf[i]))
			{
				if (!isspace(inputElements[stringElements - 1]))
				{
					inputElements[stringElements++] = ' ';
					inputElements = realloc(inputElements, stringElements * sizeof(char));
					elementCount++;
				}
			}
			else
			{
				inputElements[stringElements++] = buf[i];
				inputElements = realloc(inputElements, stringElements * sizeof(char));
			}
		}
	}
	inputElements[stringElements] = '\0';

	//printf("%d elements\n", elementCount);
	*count = elementCount;
	return (const char *)inputElements;
}