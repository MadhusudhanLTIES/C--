#include <errno.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Common.h"

int sizeOfString(char* testString)
{
	sizeof(testString) / sizeof(char*) ;
}

char* binaryToHexString(char* inputData, char* outputData, int length)
{
	fflush(stdout);
	char hexChars[] = "0123456789ABCDEF";
	int index;
	for(index = 0; index <= length; index++) 
	{
		char char1 = hexChars[((inputData[index] & 0xf0) >> 4)];
		char char2 = hexChars[(inputData[index] & 0x0f)];
		outputData[index*2] = char1;
		outputData[index*2+1] = char2;
		outputData[index*2+2] = 0;
	}
	outputData[2*length] = 0;
	return outputData;
}

char* hexStringToBinary(char* inputData, char* outputData, int* length)
{
	int index;
	int size = 0;
	for(index = 0; 1; index += 2)
	{
		if(inputData[index] == 0)
		{
			break;
		}
		size++;
		char char1 = inputData[index];
		char char2 = inputData[index+1];
		if(char1 >= 48 && char1 <= 57) 
		{
			char1 -= 48;
		}
		else if(char1 >= 65 && char1 <= 90) 
		{
			char1 -= 55;
		}
		if(char2 >= 48 && char2 <= 57) 
		{
			char2 -= 48;
		}
		else if(char2 >= 65 && char2 <= 90)
		{
			char2 -= 55;
		}
		outputData[index/2] = char1 << 4 | char2;
	}
	*length = size - 1;
	return outputData;
}

char* asciiToHexString(char* in, char* out)
{
	char hexChars[] = "0123456789ABCDEF";
	int iii = 0;
	while(1) 
	{
		if(in[iii] == 0)
		{
			out[2*iii] = 0;
			break;
		}
		else
		{
			char letter = in[iii];
			out[2*iii] = hexChars[(char)((letter >> 4) & 0x0f)];
			out[2*iii + 1] = hexChars[(char)(letter & 0x0f)];
			iii++;
		}
	}
	return out;
}
