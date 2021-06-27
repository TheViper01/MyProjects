#include "array_class.cpp"
#include <algorithm>

#define DataType int


static void OrdinateFill(int* Array, size_t size)
{
	for (size_t j = 0; j < size; j++)
	{
		Array[j] = j;
	}
}

int main()
{
	size_t len = 10;

	ArrayCls<DataType> arr_1(3, 3, 4, 100);

	OrdinateFill(arr_1.data(), arr_1.Get_TotalElements());

	arr_1.Resize(4, 4, 5, 6, 120);

	arr_1.PrintArrayAddresses();
	arr_1.~ArrayCls();

	char gg;
	printf("Press a key to exit...");
	scanf("%c", &gg);

	return 0;
}
