#include <stdio.h>

int main() {
	int AValue, *BValue;
	
	/* See what is stored in AValue at present*/
	printf("AValue = %d \n", AValue);
	
	/* Set the Value of AValue to what we want */
	AValue = 101;
	
	/* Print the memory address where AValue is stored and the
	value*/
	/* by using the "&" Reference operator*/
	printf("Address of AValue %p AValue = %d \n", & AValue, AValue);
	
	/* Save the address of AValue at a separate location*/
	BValue = &AValue;
	
	/* Print the BValue , which is the address of AValue */
	/* Dereference BValue with "*" operator and print actual Value*/
	printf("Address of BValue %p BValue = %d \n", BValue, * BValue);
	/* Note output will show BValue=101 (it was never explicitly set
	to 101) */
}
