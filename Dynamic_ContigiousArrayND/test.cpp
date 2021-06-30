#include <iostream>
#include "array_class.cpp"

#define DataType int


template <typename DataType>
static void ordinate_fill(DataType* Array, size_t size)
{
	for (size_t j = 0; j < size; j++)
	{
		Array[j] = j;
	}
}

int main()
{
	ArrayCls<DataType> arr_1(3, 4, 100);
	
	ordinate_fill(arr_1.data(), arr_1.capacity());
	
	arr_1.Resize(4, 5, 6, 120);
	std::cout << arr_1.print_array().str();
	
	arr_1.~ArrayCls();

	char gg;
	std::cout << "Press a key to exit..." << std::endl;
	std::cin >> gg;

	return 0;
}
