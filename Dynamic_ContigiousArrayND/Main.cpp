#include "array_class.cpp"
#include <algorithm>

#define DataType int

int main()
{
	size_t len = 10;

	ArrayCls<DataType> arr_1(3, 3, 4, 100);

	//DataType* Buffer = (DataType*)malloc(arr_1.Get_TotalElements() * sizeof(DataType));
	//ReadFromFile("C:/Users/BadBoy/Desktop/num.txt", arr_1.Get_TotalElements() * sizeof(DataType), Buffer, "rb");

	//memcpy(arr_1.ArrayPtr.data(), Buffer, arr_1.Get_TotalElements() * sizeof(DataType));

	OrdinateFill(arr_1.data(), arr_1.Get_TotalElements());

	Timer timer;

	/*arr_1.QuickSort();
	arr_1.sort();

	timer.Stop();*/

	arr_1.Resize(4, 4, 5, 6, 120);

	arr_1.PrintArrayAddresses();
	arr_1.~ArrayCls();

	/*RandomFill(Buffer, len);
	WriteToFile("C:/Users/BadBoy/Desktop/num.txt", len * sizeof(DataType), Buffer, "wb+");*/

	
	//free(Buffer);
	/*size_t dim[1] = {1};
	arr_1.get(1, 1) = 33;

	printf("%d\n", &arr_1.get(1, 1));
	printf("%d\n", &arr_1.get_p(1, dim));*/

	char gg;
	printf("Press a key to exit...");
	scanf("%c", &gg);

	return 0;
}