#ifndef _ARRAYCLS_H_
#define _ARRAYCLS_H_

#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <string>
#include <algorithm>

#define swap(a, b)\
	do{\
		decltype(a) temp = a;\
		a = b;\
		b = temp;\
	}while(0)


#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

#define SwitchOrder(arr_ptr, TotIteams)\
    do{\
		typename std::remove_pointer <decltype(arr_ptr)>::type temp;\
		size_t start = 0, end = TotIteams - 1;\
		while (start < end)\
		{\
			temp = arr_ptr[start];\
			arr_ptr[start] = arr_ptr[end];\
			arr_ptr[end] = temp;\
			start++;\
			end--;\
		}\
    }while(0)

#define PrintArray(Ptr, Size)\
    for(size_t j=0; j < Size; j++)\
    {\
		std::cout << "[" << j << "] = " << Ptr[j] << std::endl;\
    }


static int WriteToFile(const char* FilePath, size_t TotElements, const void* Buffer, const char* FopenMode)
{
	FILE* FilePtr = fopen(FilePath, FopenMode);
	if (!FilePtr) return 1;
	fwrite(Buffer, sizeof(char), TotElements, FilePtr);
	fclose(FilePtr);
	return 0;
}

static int ReadFromFile(const char* FilePath, size_t TotElements, void* Buffer, const char* FopenMode)
{
	FILE* FilePtr = fopen(FilePath, FopenMode);
	if (!FilePtr) return 1;
	fread(Buffer, sizeof(char), TotElements, FilePtr);
	fclose(FilePtr);
	return 0;
}

static void RandomFill(int* Array, size_t size)
{
	unsigned int seed = (unsigned int)time(0);
	
	for (size_t j = 0; j < size; j++)
	{
		seed++;
		srand(seed);
		Array[j] = rand();
		//printf("%d\n", Array[j]);
	}
}

/***************************************************************************************************************************************/
template <typename DataType>

class ArrayCls
{
public:
	DataType* ArrayPtr;

	/**************************************************************************************************************/
		///Destructor///
	~ArrayCls()
	{
		if (ArrayPtr) free(ArrayPtr);
		if (Dimensions) free(Dimensions);
		if (DimensionsProd) free(DimensionsProd);
	}
	/**************************************************************************************************************/
		///Constructor///
	ArrayCls <DataType>(unsigned short TotDimen, ...)
	{
		va_list listPointer;
		va_start(listPointer, TotDimen);

		size_t* args = (size_t*)listPointer;

		Dimensions = (size_t*)malloc(TotDimen * sizeof(size_t));

		memcpy(Dimensions, args, TotDimen * sizeof(size_t));
		va_end(listPointer);

		unsigned short DimPtr_size = TotDimen - 1;
		size_m = sizeof(DataType);
		TotDim = TotDimen;

		if (TotDimen > 1)
		{
			DimensionsProd = (size_t*)malloc(DimPtr_size * sizeof(size_t));
			///Calculating DimProd
			size_t CurrDimOffset = Dimensions[DimPtr_size];
			unsigned short counter = 0;
			for (unsigned short j = DimPtr_size; j > 0; j--)
			{
				if (Dimensions[j] == 0) this->~ArrayCls();

				DimensionsProd[counter++] = CurrDimOffset;
				CurrDimOffset *= Dimensions[j - 1];

				//std::cout << "Dimensions[" << counter - 1 << "] = " << DimensionsProd[counter - 1] << std::endl;
			}
			SwitchOrder(DimensionsProd, DimPtr_size);
			TotalElements = DimensionsProd[0] * Dimensions[0];
		}
		else if (TotDimen < 1) this->~ArrayCls();
		else
		{
			DimensionsProd = (size_t*)malloc(sizeof(size_t));
			DimensionsProd[0] = args[0];
			TotalElements = args[0];
		}
		ArrayPtr = (DataType*)malloc(TotalElements * size_m);
		//std::cout << ArrayPtr << std::endl;
	}
	/**************************************************************************************************************/
		///access an array member address with the number of dimensions that you want
	DataType& get(unsigned short TotDim_f, ...)
	{
		va_list listPointer;
		va_start(listPointer, TotDim_f);

		DataType& val = get_p(TotDim_f, (size_t*)listPointer);

		va_end(listPointer);
		return val;
	}
	/**************************************************************************************************************/
	///access an array member address with all dimensions with an array of dimensions
	DataType& get_p(unsigned short TotDim_f, size_t* args)
	{
		if (TotDim_f < 1 || TotDim_f > TotDim)		return *((DataType*) (size_t)(0));
		else if (TotDim == 1)						return *(ArrayPtr + *args);

		unsigned short TotDim_to = TotDim_f - 1;
		size_t offset;

		if (TotDim_f == TotDim)		offset = args[TotDim_to];
		else if (TotDim_f < TotDim)	offset = args[TotDim_to] * DimensionsProd[TotDim_to];

		for (unsigned short j = 0; j < TotDim_to; j++)
		{
			if (args[j] == 0) continue;
			offset += args[j] * DimensionsProd[j];
		}

		return *(ArrayPtr + offset);
	}
	/**************************************************************************************************************/
		///sort the array in an increasing order
	void QuickSort(DataType* low = 0, DataType* high = 0)
	{
		size_t low_int, high_int;

		if (!low) low_int = 0;
		else low_int = (size_t)(((size_t)low - (size_t)ArrayPtr) / sizeof(DataType));

		if (!high) high_int = TotalElements - 1;
		else high_int = (size_t)(((size_t)high - (size_t)ArrayPtr) / sizeof(DataType));

		QuickSort_Driver(low_int, high_int);
		return;
	}
	/**************************************************************************************************************/
	void sort(DataType* low = 0, DataType* high = 0)
	{
		DataType *low_int, *high_int;

		if (!low) low_int = ArrayPtr;
		else low_int = low;

		if (!high) high_int = ArrayPtr + TotalElements - 1;
		else high_int = high;

		std::sort(low_int, high_int);
		return;
	}
	/**************************************************************************************************************/
	bool IsSorted(DataType* low = 0, DataType* high = 0)
	{
		size_t low_int, high_int;

		if (!low) low_int = 0;
		else low_int = (size_t)(((size_t)low - (size_t)ArrayPtr) / sizeof(DataType));

		if (!high) high_int = TotalElements - 1;
		else high_int = (size_t)(((size_t)high - (size_t)ArrayPtr) / sizeof(DataType));

		return IsSorted_Driver(low_int, high_int);
	}
	/**************************************************************************************************************/
	void Reverse(DataType* low = 0, DataType* high = 0)
	{
		size_t low_int, high_int;

		if (!low) low_int = 0;
		else low_int = (size_t)(((size_t)low - (size_t)ArrayPtr) / sizeof(DataType));

		if (!high) high_int = TotalElements - 1;
		else high_int = (size_t)(((size_t)high - (size_t)ArrayPtr) / sizeof(DataType));

		DataType temp;
		while (low_int < high_int)
		{
			temp = ArrayPtr[low_int];
			ArrayPtr[low_int] = ArrayPtr[high_int];
			ArrayPtr[high_int] = temp;
			low_int++;
			high_int--;
		}
	}
	/**************************************************************************************************************/
	char Resize(unsigned short TotDim_f, ...)
	{
		va_list listPointer;
		va_start(listPointer, TotDim_f);

		if (TotDim_f < 1)
		{
			va_end(listPointer);
			return 3;
		}
		size_t* args = (size_t*)listPointer;

		char returnVal = Resize_p(TotDim_f, args);

		return returnVal;
	}
	/**************************************************************************************************************/
		///Expand the array and copying the members
	char Resize_p(unsigned short TotDim_f, size_t* Dimensions_p)
	{
		if (TotDim_f < 1) return 3;

		size_t* Dimensions_f = (size_t*)malloc(TotDim_f * sizeof(size_t));
		memcpy(Dimensions_f, Dimensions_p, TotDim_f * sizeof(size_t));

		///Calculate new DimensionsProd_f
		size_t len;
		size_t* DimensionsProd_f;
		if (TotDim_f > 1)
		{
			unsigned short DimPtr_size = TotDim_f - 1;
			DimensionsProd_f = (size_t*)malloc(DimPtr_size * sizeof(size_t));
			size_t CurrDimOffset;
			unsigned short counter = 0;

			CurrDimOffset = Dimensions_f[DimPtr_size];
			for (unsigned short j = DimPtr_size; j > 0; j--)
			{
				if (Dimensions_f[j] == 0) return 2;

				DimensionsProd_f[counter++] = CurrDimOffset;
				CurrDimOffset *= Dimensions_f[j - 1];
			}

			///Calculate new array length
			len = DimensionsProd_f[TotDim_f - 2] * Dimensions_f[0];
		}
		else
		{
			len = Dimensions_f[0];
			DimensionsProd_f = (size_t*)malloc(sizeof(size_t));
			DimensionsProd_f[0] = Dimensions_f[0];
		}

		DataType* array_temp = (DataType*)malloc(len * size_m);

		///1 dimension case
		if (MIN(TotDim_f, TotDim) == 1) memcpy(array_temp, ArrayPtr, MIN(DimensionsProd_f[0], DimensionsProd[0]));

		///dimensions>1 dimension case
		else
		{
			///Calculate array dimensions differences
			unsigned short Lowest_arr = MIN(TotDim, TotDim_f);
			size_t BlockMemory = MIN(Dimensions_f[0], Dimensions[0]) * DimensionsProd[0];
			SwitchOrder(Dimensions_f, TotDim_f);
			SwitchOrder(Dimensions, TotDim);
			SwitchOrder(DimensionsProd, (TotDim - 1));

			unsigned short Lowest_arr_1 = Lowest_arr - 1;
			unsigned short FirstDifference = Lowest_arr_1;
			size_t BlockMembersJmp_src;

			for (unsigned short j = 0; j < Lowest_arr_1; j++)
			{
				if (Dimensions_f[j] != Dimensions[j])
				{
					BlockMemory = MIN(DimensionsProd[j], DimensionsProd_f[j]) * sizeof(DataType);
					FirstDifference = j;
					BlockMembersJmp_src = DimensionsProd[j];
					break;
				}
			}

			///same dimensions but more of them
			if (FirstDifference == Lowest_arr_1) memcpy(array_temp, ArrayPtr, BlockMemory);
			///other combinations
			else
			{
				///Calculate minimal dimensions
				size_t* Dimensions_min = (size_t*)calloc(Lowest_arr, sizeof(size_t));

				for (unsigned short j = 0; j < Lowest_arr; j++)
				{
					Dimensions_min[j] = MIN(Dimensions[j], Dimensions_f[j]);
				}

				///Memcpy the array
				size_t* Dimensions_final = (size_t*)calloc(Lowest_arr, sizeof(size_t));
				size_t JumpOffset_src = 0, JumpOffset_dest = 0;
				unsigned short FirstDifference_1 = FirstDifference + 1;

				while (1)
				{
					memcpy(&array_temp[JumpOffset_dest], &ArrayPtr[JumpOffset_src], BlockMemory);

					Dimensions_final[FirstDifference_1]++;

					for (unsigned short j = FirstDifference_1; j < Lowest_arr_1; j++)
					{
						if (Dimensions_final[j] == Dimensions_min[j])
						{
							Dimensions_final[j] = 0;
							Dimensions_final[j + 1] += 1;
						}
					}

					if (Dimensions_final[Lowest_arr_1] >= Dimensions_min[Lowest_arr_1]) break;

					JumpOffset_src += BlockMembersJmp_src;
					JumpOffset_dest = OffsetCalculator(Dimensions_final, DimensionsProd_f, Lowest_arr);
				}
				free(Dimensions_final);
				free(Dimensions_min);
			}
			SwitchOrder(DimensionsProd_f, (TotDim_f - 1));
			SwitchOrder(Dimensions_f, TotDim_f);
		}
		free(ArrayPtr);
		free(Dimensions);
		free(DimensionsProd);

		ArrayPtr = array_temp;
		Dimensions = Dimensions_f;
		DimensionsProd = DimensionsProd_f;
		TotDim = TotDim_f;
		TotalElements = len;

		return 0;
	}
	/**************************************************************************************************************/
	void PrintArrayAddresses()
	{
		size_t* Dimensions_final = (size_t*)calloc(TotDim, sizeof(size_t));
		unsigned short TotDim_1 = TotDim - 1;
		while (1)
		{
			for (unsigned short j = 0; j < TotDim; j++)
			{
				std::cout << (size_t)Dimensions_final[j] << " ";
			}
			std::cout << "= " << (size_t)get_p(Dimensions_final) << std::endl;

			Dimensions_final[TotDim_1] += 1;

			for (unsigned short j = TotDim_1; j > 0; j--)
			{
				if (Dimensions_final[j] == Dimensions[j])
				{
					Dimensions_final[j] = 0;
					Dimensions_final[j - 1] += 1;
				}
			}
			//printf("%d %d\n", Dimensions_final[2], Dimensions[2]);
			if (Dimensions_final[0] >= Dimensions[0]) break;
		}
		free(Dimensions_final);
	}
	/**************************************************************************************************************/
	void PrintArrayElements()
	{
		size_t* Dimensions_final = (size_t*)calloc(TotDim, sizeof(size_t));
		unsigned short TotDim_1 = TotDim - 1;
		while (1)
		{
			for (unsigned short j = 0; j < TotDim; j++)
			{
				std::cout << (size_t)Dimensions_final[j] << " ";
			}
			std::cout << "= " << get_p(TotDim_1, Dimensions_final) << std::endl;

			Dimensions_final[TotDim_1] += 1;

			for (unsigned short j = TotDim_1; j > 0; j--)
			{
				if (Dimensions_final[j] == Dimensions[j])
				{
					Dimensions_final[j] = 0;
					Dimensions_final[j - 1] += 1;
				}
			}
			//printf("%d %d\n", Dimensions_final[2], Dimensions[2]);
			if (Dimensions_final[0] >= Dimensions[0]) break;
		}
		if(!Dimensions_final) free(Dimensions_final);
	}
	/**************************************************************************************************************/
	void fill(DataType* Value)
	{
		for (size_t j = 0; j < TotalElements; j++)
		{
			ArrayPtr[j] = *Value;
		}
	}
	/***************************************************************************************************************************************/
	size_t Get_TotalElements(void)
	{
		return TotalElements;
	}
	/***************************************************************************************************************************************/
	size_t Get_SizeMember(void)
	{
		return sizeof(DataType);
	}
	/***************************************************************************************************************************************/
	unsigned short Get_TotDim(void)
	{
		return TotDim;
	}
	/***************************************************************************************************************************************/
	size_t* Get_Dimensions(void)
	{
		size_t len = sizeof(size_t) * TotDim;
		size_t* temp_ptr = (size_t*)malloc(len);
		memcpy(temp_ptr, Dimensions, len);
		return temp_ptr;
	}
	/***************************************************************************************************************************************/
	size_t* Get_DimensionsProd(void)
	{
		size_t len = sizeof(size_t) * TotDim - 1;
		size_t* temp_ptr = (size_t*)malloc(len);
		memcpy(temp_ptr, DimensionsProd, len);
		return temp_ptr;
	}
	/***************************************************************************************************************************************/
private:
	size_t size_m;
	unsigned short TotDim;
	size_t* Dimensions;
	size_t* DimensionsProd;
	size_t TotalElements;

	///used in expand functions with the arguments array switched order
	size_t OffsetCalculator(size_t* Dimensions_togo, size_t* DimensionsProd_f, unsigned short TotDim_f)
	{
		size_t offset = 0;
		for (unsigned short j = 1; j < TotDim_f; j++)
		{
			if (Dimensions_togo[j] == 0) continue;
			offset += Dimensions_togo[j] * DimensionsProd_f[j - 1];
		}
		return offset;
	}
	/**************************************************************************************************************/
	size_t PartitionQuickSort(size_t low, size_t high)
	{
		DataType pivot = ArrayPtr[high]; // pivot  
		size_t PIndex = low; // Index of smaller element  
		DataType temp;

		for (size_t j = low; j < high; j++)
		{
			// If current element is smaller than the pivot  
			if (ArrayPtr[j] <= pivot)
			{
				temp = ArrayPtr[PIndex];
				ArrayPtr[PIndex] = ArrayPtr[j];
				ArrayPtr[j] = temp;

				PIndex++; // increment index of smaller element
			}
		}
		temp = ArrayPtr[PIndex];
		ArrayPtr[PIndex] = ArrayPtr[high];
		ArrayPtr[high] = temp;

		return PIndex;
	}
	/**************************************************************************************************************/
	void QuickSort_Driver(size_t low, size_t high)
	{
		while (low < high)
		{
			/* pi is partitioning index, arr[p] is nowat right place */
			size_t PIndex = PartitionQuickSort(low, high);

			// If left part is smaller, then recur for left part and handle right part iteratively 
			if (PIndex - low < high - PIndex)
			{
				if (PIndex > 0) QuickSort_Driver(low, PIndex - 1);
				low = PIndex + 1;
			}
			// Else recur for right part 
			else
			{
				QuickSort_Driver(PIndex + 1, high);
				high = PIndex - 1;
			}
		}
	}
	/**************************************************************************************************************/
	bool IsSorted_Driver(size_t low, size_t high)
	{
		if (low > high) return false;

		// Array has one or no element 
		if ((high - low) < 2) return true;

		for (size_t j = low; j < high; j++)

			// Unsorted pair found 
			if (ArrayPtr[j] > ArrayPtr[j + 1])
				return false;

		// No unsorted pair found 
		return true;
	}
	/**************************************************************************************************************/
};


/**********************************************************************************************************************************************************************/
class Timer
{
public:
	long long TimePassed;
	double SecondsPassed;

	Timer()
	{
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}

	void Stop(void)
	{
		auto endTimepoint = std::chrono::high_resolution_clock::now();

		auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
		auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

		auto duration = end - start;
		double sec = duration * 0.000001;

		TimePassed = duration;
		SecondsPassed = sec;

		std::cout << "Time taken seconds: " << sec << "\n";
	}
private:
	std::chrono::time_point< std::chrono::high_resolution_clock> m_StartTimepoint;
};

#endif	//_ARRAYCLS_H_
