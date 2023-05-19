/*
Rohan Kallur
I pledge my honor that I have abided by the Stevens Honor System.
*/
#include <stdlib.h>
#include <string.h>
#include "utils.h"

/*
	You are free to use any data type you like in this file.
	However, other than the five functions declared in "utils.h",
	DO NOT create any other functions.

*/

int cmpr_int(void *x, void *y)
{
	// cast the parameters into ints and compare normally
	int *casted_x = (int *)x;
	int *casted_y = (int *)y;
	if (*casted_x == *casted_y)
	{
		return 0;
	}
	else if (*casted_x > *casted_y)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

int cmpr_float(void *x, void *y)
{
	// cast the parameters into floaters and compare normally
	float *casted_x = (float *)x;
	float *casted_y = (float *)y;
	if (*casted_x == *casted_y)
	{
		return 0;
	}
	else if (*casted_x > *casted_y)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

void print_int(void *x)
{
	// print by using %d and casting the parameter to an int
	printf("%d\n", *(int *)x);
}

void print_float(void *x)
{
	// print by using %f and casting the parameter to a float
	printf("%f\n", *(float *)x);
}

void *read_array(char *filename, char *format, size_t *len)
{
	FILE *fp = fopen(filename, "r"); // create a file pointer to store the file in read mode
	if (fp == NULL)					 // check if the file exists/opens correctly
	{
		fprintf(stderr, "File failed to open.\n");
		exit(EXIT_FAILURE);
	}
	char buffer[255];
	// determine type to create array of proper type.
	while (fgets(buffer, 255, fp))
	{
		(*len)++;
	}
	fclose(fp);
	fp = fopen(filename, "r");
	if (strcmp(format, "%d") == 0)
	{
		int *arr = malloc(sizeof(int) * (*len));
		memset(arr, 0, sizeof(int) * (*len));
		for (unsigned int i = 0; i < *len; i++)
		{
			fgets(buffer, 255, fp);
			arr[i] = atoi(buffer);
		}
		int *ptr = arr;
		fclose(fp);
		return ptr;
	}
	else
	{
		float *arr = malloc(sizeof(float) * (*len));
		memset(arr, 0, sizeof(float) * (*len));
		for (unsigned int i = 0; i < *len; i++)
		{
			fgets(buffer, 255, fp);
			arr[i] = atof(buffer);
		}
		float *ptr = arr;
		fclose(fp);
		return ptr;
	}
}