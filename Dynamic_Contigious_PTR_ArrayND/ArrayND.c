#ifndef _ARRAYND_H_
#define _ARRAYND_H_

#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>
//#include <iostream>

//#define swap(a, b)\
//	do{\
//		auto temp = a;\
//		a = b;\
//		b = temp;\
//	}while(0)

/**********************************************************************************************************************/
void** array_2d(size_t _row, size_t _col, size_t _size_m)
{
	char *ptr, **arr;
	size_t i;

	arr = (char**)malloc(sizeof(char*) * _row + _size_m * _col * _row);
	if (!arr) return 0; //Memory allocation failed

	// ptr is now pointing to the first element in of 2D array
	ptr = (char*)(arr + _row);

	// for loop to point rows pointer to appropriate location in 2D array
	for (i = 0; i < _row; i++)
	{
		arr[i] = ((char*)(ptr + _col * i * _size_m));
	}
	return (void**)arr;
}
/**********************************************************************************************************************/
void* arrayND(size_t size_m, unsigned short numargs, ...)
{
	va_list listPointer;
	va_start(listPointer, numargs);

	if (numargs < 1 || size_m < 1) return 0;

	size_t* args = (size_t*)listPointer;

	if (!args[0] || !args[numargs - 1])	///Dimension == 0 case
	{
		va_end(listPointer);
		return 0;
	}

	if (numargs == 1)
	{
		void* array = (void*)malloc(size_m * args[0]);
		assert(array);
		va_end(listPointer);
		return array;
	}

	size_t* PtrProdSum = (size_t*)malloc(((numargs - 1) * 2) * sizeof(size_t));
	assert(PtrProdSum);
	size_t* PtrProd = &PtrProdSum[numargs - 1];

	if (!PtrProdSum) return 0;

	PtrProdSum[0] = args[0];
	PtrProd[0] = args[0];

	for (unsigned short j = 1; j < numargs - 1; j++)      ///Calculating the number of (*) pointers and double pointers (**)
	{
		if (args[j] < 1)	///Dimension < 1 case
		{
			free(PtrProdSum);
			free(PtrProd);
			va_end(listPointer);
			return 0;
		}
		PtrProd[j] = PtrProd[j - 1] * args[j];
		PtrProdSum[j] = PtrProd[j] + PtrProdSum[j - 1];
	}

	size_t len = (sizeof(void*) * PtrProdSum[numargs - 2]) + (size_m * PtrProd[numargs - 2] * args[numargs - 1]);
	void** array = (void**)malloc(len);      ///assign a big block of memory
	assert(array);

	if (!array) return 0;

	unsigned short j = 0;		///j = number block of usable memory

	size_t PPprodSum, PprodSum = PtrProdSum[numargs - 2];
	size_t ColMemory = size_m * args[numargs - 1];
	char* ptr = (char*)(array + PprodSum);

	///assigning array addresses
	if (numargs > 2)
	{
		unsigned short k = 1;		///k = index of args to process the addresses
		PPprodSum = PtrProdSum[numargs - 3];
		size_t last_jump = args[0];

		for (size_t i = 0; i < PPprodSum; i++)		///assigning (**) double pointers addresses
		{
			array[i] = (void**)array + (last_jump + i);
			//printf("array[%d]  :  %d  to  %d   last_jump = %d   PtrProdSum = %d   args = %d\n", i, &array[i], array[i], last_jump, PtrProdSum[k], args[k]);

			if (i >= PtrProdSum[k - 1] && k < numargs - 2) k++;
			last_jump += args[k] - 1;
		}
	}
	else PPprodSum = 0;

	va_end(listPointer);
	free(PtrProdSum);

	for (size_t i = PPprodSum; i < PprodSum; i++)		///assigning (*) pointers to members memory addresses
	{
		array[i] = ptr + ColMemory * j;
		//printf("array[%d] = %d    jump = %d    sum = %d\n", i, &array[i], ptr + (ColMemory * j), (ColMemory * j));
		j++;
	}

	return (void*)array;
}

#endif // !_ARRAYND_H_
