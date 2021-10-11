#include <stdio.h>

int main() {

	int a[] = {1,2,3,4,5,6,7,8,9,10};
	
	for (int i = 0; i < 10; i++) {
		int remainder = a[i] % 2;
		
		switch(remainder) 
		{
			case 0: 
			{
				printf("%d is an even number\n", a[i]);
				break;
			}
			case 1: 
			{
				printf("%d is an odd number\n", a[i]);
				break;
			}
			default: 
			{
				printf("default value");
				break;
			}
		}
	}
	
	return 0;
}
