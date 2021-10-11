#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<stdbool.h>
#include <time.h>

bool isPrime(int n);

int main(int argc, char **argv)
{
	
	int n, i;
	
	int *primeNumbers;
	int primeCounter = 0;
	
	struct timespec start, end, start_s1, end_s1, start_s2, end_s2;
	double time_taken; 
	
	// Get current clock time.
	clock_gettime(CLOCK_MONOTONIC, &start); 
	
	FILE *fp;	
	
	// Part-1 Read Input
	sscanf(argv[1], "%d", &n);
	
	primeNumbers = malloc(n *sizeof(int)); // dynamically allocate size to the pointer variable
	
	// Part-2 Check prime from 2 to n
	for(i=2; i<n; i++){
		if(isPrime(i)){
			primeNumbers[primeCounter] = i;
			primeCounter++;
		
		}
	}
	
	// Part-3 Output to text file
	fp = fopen("primes.txt","w");
	for(i=0;i<primeCounter;i++){
		fprintf(fp, "%d\n", primeNumbers[i]);
	
	}
	fclose(fp);

	// Get the clock current time again
	// Subtract end from start to get the CPU time used.
	clock_gettime(CLOCK_MONOTONIC, &end); 
	time_taken = (end.tv_sec - start.tv_sec) * 1e9; 
    time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9; 
	printf("Overall time (Including read, product and write)(s): %lf\n", time_taken);	// ts

	return 0;
}

bool isPrime(int n){
    int j;
	// Operation to check if the given number is prime 
	for(j=2; j<=sqrt(n); j++){
		if(n%j==0){
			return false;
		}
	}
	
	return true;
}


