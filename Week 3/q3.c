#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<stdbool.h>
#include <time.h>
#include<pthread.h>
#define NUMTHREAD 4

// Global Variables
int *primeNumbers;
int primeCounter; 
int n;

bool isPrime(int n);
void *searchPrimeFunction(void *arg);

pthread_mutex_t gMutex;

int main(){
	
	pthread_t hThread[NUMTHREAD];
	int threadNum[NUMTHREAD];
	
	pthread_mutex_init(&gMutex, NULL);
	
	struct timespec start, end, start_s1, end_s1, start_s2, end_s2, startComp, endComp;
	double time_taken; 
	
	clock_gettime(CLOCK_MONOTONIC, &start); 
	
	FILE *fp;	
	
	// Part-1 Read Input
	printf("Input an integer number:");
	scanf("%d", &n);
	
	primeNumbers = malloc(n *sizeof(int)); // dynamically allocate size to the pointer variable
	primeCounter = 0;
	
	clock_gettime(CLOCK_MONOTONIC, &startComp); 
	
	// Part-2A Create Thread to do parallel task
	for(int i=0;i<NUMTHREAD;i++){
		threadNum[i] = i;
		pthread_create(&hThread[i], NULL, searchPrimeFunction, &threadNum[i]);
	}	
	
	// Part-2B Join the threads
	for(int i=0;i<NUMTHREAD;i++){
		pthread_join(hThread[i], NULL);
	}
	
	// Get the clock current time again
	// Subtract end from start to get the CPU time used.
	clock_gettime(CLOCK_MONOTONIC, &endComp); 
	time_taken = (endComp.tv_sec - startComp.tv_sec) * 1e9; 
    time_taken = (time_taken + (endComp.tv_nsec - startComp.tv_nsec)) * 1e-9; 
	printf("Overall time (product only)(s): %lf\n", time_taken);
	
	// Part-3 Output to text file
	fp = fopen("primes.txt","w");
	for(int i=0;i<primeCounter;i++){
		fprintf(fp, "%d\n", primeNumbers[i]);
	
	}
	fclose(fp);
	
	// Get the clock current time again
	// Subtract end from start to get the CPU time used.
	clock_gettime(CLOCK_MONOTONIC, &end); 
	time_taken = (end.tv_sec - start.tv_sec) * 1e9; 
    time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9; 
	printf("Overall time (Including read, product and write)(s): %lf\n", time_taken);

	return 0;
}

void *searchPrimeFunction(void *arg){
    
	int pid = *(int*) arg;
	
	int rpt = n / NUMTHREAD;
	int rptr = n % NUMTHREAD;
	
	int start = pid * rpt;
	int end = start + rpt;
	
	if (start == 0) {
	    start += 2;
	}
	
	if (pid == NUMTHREAD - 1) {
	    end += rptr;
	}
	
	for(int i=start; i<end; i++){
		if(isPrime(i)){
			pthread_mutex_lock(&gMutex);
			primeNumbers[primeCounter] = i;
			primeCounter++;
			pthread_mutex_unlock(&gMutex);	
		}
	}
}


bool isPrime(int n){

	// Operation to check if the given number is prime 
	for(int j=2; j<=sqrt(n); j++){
		if(n%j==0){
			return false;
		}
	}
	
	return true;
}


