#ifndef _ARRAYCLS_CPP_
#define _ARRAYCLS_CPP_

#include <iostream>
#include <algorithm>
#include <vector>
#include <sstream> // for ostringstream


/***************************************************************************************************************************************/
template <typename DataType>
class arrayND
{
public:

	/**************************************************************************************************************/
	///Destructor///
	~arrayND()
	{
		ArrayPtr.~vector();
		Dimensions.~vector();
		DimensionsProd.~vector();
	}
	/**************************************************************************************************************/
	///Constructor///
	arrayND <DataType>()
	{
		TotDim = 0;
		TotalElements = 0;
	}
	template<typename... Dim>
	arrayND <DataType>(Dim... dim)
	{
		size_t dimensions_f[] = {dim...};
		init_p(_countof(dimensions_f), dimensions_f);
	}
	/**************************************************************************************************************/
	DataType* data()
	{
		return(ArrayPtr.data());
	}
	/**************************************************************************************************************/
	///access an array member address with the number of dimensions that you want
	template<typename... Dim>
	DataType& at(Dim... dim)
	{
		size_t dimensions_f[] = {dim...};
		DataType& val = at_p(_countof(dimensions_f), dimensions_f);
		return val;
	}
	/**************************************************************************************************************/
	///access an array member address with all dimensions with an array of dimensions
	DataType& at_p(unsigned short TotDim_f, size_t* args)
	{
		if (TotDim_f < 1 || TotDim_f > TotDim)		return *((DataType*)0);
		else if (TotDim == 1)						return ArrayPtr[*args];

		unsigned short TotDim_to = TotDim_f - 1;
		size_t offset;

		if (TotDim_f == TotDim)		offset = args[TotDim_to];
		else if (TotDim_f < TotDim)	offset = args[TotDim_to] * DimensionsProd[TotDim_to];

		for (unsigned short j = 0; j < TotDim_to; j++)
		{
			if (args[j] == 0) continue;
			offset += args[j] * DimensionsProd[j];
		}

		return ArrayPtr[offset];
	}
	/**************************************************************************************************************/
		///sort the array in an increasing order
	void quicksort(DataType* low = 0, DataType* high = 0)
	{
		size_t low_int, high_int;

		if (!low) low_int = 0;
		else low_int = (size_t)(((size_t)low - (size_t)ArrayPtr.data()) / sizeof(DataType));

		if (!high) high_int = TotalElements - 1;
		else high_int = (size_t)(((size_t)high - (size_t)ArrayPtr.data()) / sizeof(DataType));

		quicksort_driver(low_int, high_int);
		return;
	}
	/**************************************************************************************************************/
	void sort(DataType* low = 0, DataType* high = 0)
	{
		DataType* low_int, * high_int;

		if (!low) low_int = ArrayPtr.data();
		else low_int = low;

		if (!high) high_int = ArrayPtr.data() + TotalElements - 1;
		else high_int = high;

		std::sort(low_int, high_int);
		return;
	}
	/**************************************************************************************************************/
	bool is_sorted(DataType* low = 0, DataType* high = 0)
	{
		size_t low_int, high_int;

		if (!low) low_int = 0;
		else low_int = (size_t)(((size_t)low - (size_t)ArrayPtr.data()) / sizeof(DataType));

		if (!high) high_int = TotalElements - 1;
		else high_int = (size_t)(((size_t)high - (size_t)ArrayPtr.data()) / sizeof(DataType));

		return is_sorted_driver(low_int, high_int);
	}
	/**************************************************************************************************************/
	void reverse(DataType* low = 0, DataType* high = 0)
	{
		size_t low_int, high_int;

		if (!low) low_int = 0;
		else low_int = (size_t)(((size_t)low - (size_t)ArrayPtr.data()) / sizeof(DataType));

		if (!high) high_int = TotalElements - 1;
		else high_int = (size_t)(((size_t)high - (size_t)ArrayPtr.data()) / sizeof(DataType));

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
	template<typename... Dim>
	void init(Dim... dim)
	{
		size_t dimensions_f[] = {dim...};
		init_p(_countof(dimensions_f), dimensions_f);
	}
	/**************************************************************************************************************/
	void init_p(unsigned short TotDim_f, size_t* Dimensions_p)
	{
		size_t* args = (size_t*)Dimensions_p;

		Dimensions.resize(TotDim_f);
		Dimensions.shrink_to_fit();
		Dimensions.assign(args, args + (TotDim_f));

		unsigned short DimPtr_size = TotDim_f - 1;

		if (TotDim_f > 1)
		{
			DimensionsProd.resize(TotDim_f-1);
			DimensionsProd.shrink_to_fit();

			///Calculating DimProd
			size_t CurrDimOffset = Dimensions[DimPtr_size];
			unsigned short counter = 0;
			for (unsigned short j = DimPtr_size; j > 0; j--)
			{
				if (Dimensions[j] == 0)
				{
					Dimensions.resize(0);
					Dimensions.shrink_to_fit();
					DimensionsProd.resize(0);
					DimensionsProd.shrink_to_fit();
					return;
				}
				DimensionsProd[counter++] = CurrDimOffset;
				CurrDimOffset *= Dimensions[j - 1];

				//std::cout << "Dimensions[" << counter - 1 << "] = " << DimensionsProd[counter - 1] << std::endl;
			}
			std::reverse(DimensionsProd.begin(), DimensionsProd.end());
			TotalElements = DimensionsProd[0] * Dimensions[0];
		}
		else if (TotDim_f < 1)
		{
			TotDim = 0;
			TotalElements = 0;
			return;
		}
		else
		{
			DimensionsProd.resize(0);
			DimensionsProd.shrink_to_fit();

			//DimensionsProd[0] = args[0];
			TotalElements = args[0];
		}
		TotDim = TotDim_f;
		ArrayPtr.resize(TotalElements);
		ArrayPtr.shrink_to_fit();
	}
	/**************************************************************************************************************/
	template<typename... Dim>
	void resize(Dim... dim)
	{
		size_t dimensions_f[] = {dim...};
		resize_p(_countof(dimensions_f), dimensions_f);
	}
	/**************************************************************************************************************/
		///Expand the array and copying the members
	void resize_p(unsigned short TotDim_f, size_t* Dimensions_p)
	{
		if (TotDim_f < 1)
		{
			Dimensions.resize(0);
			Dimensions.shrink_to_fit();
			DimensionsProd.resize(0);
			DimensionsProd.shrink_to_fit();
			ArrayPtr.resize(0);
			ArrayPtr.shrink_to_fit();
			TotDim = 0;
			TotalElements = 0;
			return;
		}
		if (TotDim == 0)
		{
			init_p(TotDim_f, Dimensions_p);
			return;
		}

		std::vector<size_t> Dimensions_f;
		Dimensions_f.resize(TotDim_f);
		Dimensions_f.shrink_to_fit();
		Dimensions_f.assign(Dimensions_p, Dimensions_p + (TotDim_f));

		///Calculate new DimensionsProd_f
		size_t len;
		std::vector<size_t> DimensionsProd_f;
		if (TotDim_f > 1)
		{
			unsigned short DimPtr_size = TotDim_f - 1;
			DimensionsProd_f.resize(DimPtr_size);
			size_t CurrDimOffset;
			unsigned short counter = 0;

			CurrDimOffset = Dimensions_f[DimPtr_size];
			for (unsigned short j = DimPtr_size; j > 0; j--)
			{
				if (Dimensions_f[j] == 0)
				{
					Dimensions.resize(0);
					Dimensions.shrink_to_fit();
					DimensionsProd.resize(0);
					DimensionsProd.shrink_to_fit();
					ArrayPtr.resize(0);
					ArrayPtr.shrink_to_fit();
					TotDim = 0;
					TotalElements = 0;
					return;
				}

				DimensionsProd_f[counter++] = CurrDimOffset;
				CurrDimOffset *= Dimensions_f[j - 1];
			}

			///Calculate new array length
			len = DimensionsProd_f[TotDim_f - 2] * Dimensions_f[0];
		}
		else
		{
			len = Dimensions_f[0];
			DimensionsProd_f.resize(0);
			DimensionsProd_f.shrink_to_fit();
		}

		std::vector<DataType> array_temp;
		array_temp.resize(len);
		array_temp.shrink_to_fit();

		///1 dimension case
		if (std::min(TotDim_f, TotDim) == 1)
		{
			std::copy(ArrayPtr.begin(), ArrayPtr.begin() + std::min(Dimensions_f[TotDim_f-1], Dimensions[TotDim-1]), array_temp.begin());
		}

		///dimensions>1 dimension case
		else
		{
			///Calculate array dimensions differences
			unsigned short Lowest_arr = std::min(TotDim, TotDim_f);
			size_t BlockMemory = std::min(Dimensions_f[0], Dimensions[0]) * DimensionsProd[0];
			std::reverse(Dimensions_f.begin(), Dimensions_f.end());
			std::reverse(Dimensions.begin(), Dimensions.end());
			std::reverse(DimensionsProd.begin(), DimensionsProd.end());

			unsigned short Lowest_arr_1 = Lowest_arr - 1;
			unsigned short FirstDifference = Lowest_arr_1;
			size_t BlockMembersJmp_src;

			for (unsigned short j = 0; j < Lowest_arr_1; j++)
			{
				if (Dimensions_f[j] != Dimensions[j])
				{
					BlockMemory = std::min(DimensionsProd[j], DimensionsProd_f[j]);
					FirstDifference = j;
					BlockMembersJmp_src = DimensionsProd[j];
					break;
				}
			}

			///same dimensions but more of them
			if (FirstDifference == Lowest_arr_1)
			{
				std::copy(ArrayPtr.begin(), ArrayPtr.begin() + BlockMemory, array_temp.begin());
			}
			///other combinations
			else
			{
				///Calculate minimal dimensions
				std::vector<size_t> Dimensions_min;
				Dimensions_min.resize(Lowest_arr); //memset(0)

				for (unsigned short j = 0; j < Lowest_arr; j++)
				{
					Dimensions_min[j] = std::min(Dimensions[j], Dimensions_f[j]);
				}

				///Memcpy the array
				std::vector<size_t> Dimensions_final;
				Dimensions_final.resize(Lowest_arr); //memset(0)
				size_t JumpOffset_src = 0, JumpOffset_dest = 0;
				unsigned short FirstDifference_1 = FirstDifference + 1;

				while (1)
				{
					std::copy(ArrayPtr.begin() + JumpOffset_src, ArrayPtr.begin() + JumpOffset_src + BlockMemory, array_temp.begin() + JumpOffset_dest);

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
					JumpOffset_dest = OffsetCalculator(Dimensions_final.data(), DimensionsProd_f.data(), Lowest_arr);
				}
				Dimensions_final.~vector();
				Dimensions_min.~vector();
			}
			std::reverse(DimensionsProd_f.begin(), DimensionsProd_f.end());
			std::reverse(Dimensions_f.begin(), Dimensions_f.end());
		}

		ArrayPtr = array_temp;
		Dimensions = Dimensions_f;
		DimensionsProd = DimensionsProd_f;
		TotDim = TotDim_f;
		TotalElements = len;
	}
	/**************************************************************************************************************/
	std::ostringstream print_array()
	{
		std::ostringstream str_stream;
		str_stream << "Total dimensions: " << TotDim << std::endl;
		str_stream << "Dimensions: ";

		for (unsigned short j = 0; j < TotDim; j++)
		{
			str_stream << "[" << (size_t)Dimensions[j] << "]";
		}
		str_stream << std::endl;
		str_stream << "Total elements: " << TotalElements << std::endl;
		str_stream << "Size of element (bytes): " << sizeof(DataType) << std::endl;
		str_stream << "Total size (bytes): " << TotalElements * sizeof(DataType) << std::endl;

		if (TotalElements <= 0) return str_stream;

		std::vector<size_t> Dimensions_final;
		Dimensions_final.resize(TotDim);
		unsigned short TotDim_1 = TotDim - 1;
		while (1)
		{
			for (unsigned short j = 0; j < TotDim; j++)
			{
				str_stream << "[" << (size_t)Dimensions_final[j] << "]";
			}
			DataType &return_value = at_p(TotDim, Dimensions_final.data());
			str_stream << " = Addr: " << (size_t)&return_value << "   Value: " << return_value << std::endl;

			Dimensions_final[TotDim_1] += 1;

			for (unsigned short j = TotDim_1; j > 0; j--)
			{
				if (Dimensions_final[j] == Dimensions[j])
				{
					Dimensions_final[j] = 0;
					Dimensions_final[j - 1] += 1;
				}
			}
			if (Dimensions_final[0] >= Dimensions[0]) break;
		}
		return str_stream;
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
	size_t capacity(void)
	{
		return TotalElements;
	}
	/***************************************************************************************************************************************/
	unsigned short tot_dim(void)
	{
		return TotDim;
	}
	/***************************************************************************************************************************************/
	std::vector<size_t> dimensions(void)
	{
		return Dimensions;
	}
	/***************************************************************************************************************************************/
private:
	std::vector<DataType> ArrayPtr;
	unsigned short TotDim;
	std::vector<size_t> Dimensions;
	std::vector<size_t> DimensionsProd;
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
	size_t partition_quicksort(size_t low, size_t high)
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
	void quicksort_driver(size_t low, size_t high)
	{
		while (low < high)
		{
			/* pi is partitioning index, arr[p] is nowat right place */
			size_t PIndex = partition_quicksort(low, high);

			// If left part is smaller, then recur for left part and handle right part iteratively 
			if (PIndex - low < high - PIndex)
			{
				if (PIndex > 0) quicksort_driver(low, PIndex - 1);
				low = PIndex + 1;
			}
			// Else recur for right part 
			else
			{
				quicksort_driver(PIndex + 1, high);
				high = PIndex - 1;
			}
		}
	}
	/**************************************************************************************************************/
	bool is_sorted_driver(size_t low, size_t high)
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

#endif	//_ARRAYCLS_CPP_
