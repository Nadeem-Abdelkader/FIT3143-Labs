#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include<mpi.h>
#include<math.h>
#include<stdbool.h>
#include<time.h>

bool isPrime(int n);
void writeToFile(char *pFilename, int *pArray, int counter);

int main(int argc, char **argv)
{

    int rank, n, size, start, end, elementsPerProcess, primeCounter, i, offset;
    int *primeNumbers;
    primeCounter = 0;
    MPI_Status status;
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    
    //Clock variables
    struct timespec start_t, end_t, startComp, endComp;
	double time_taken;
	
	// Get current clock time.
     clock_gettime(CLOCK_MONOTONIC, &start_t);
	
        
    // STEP - 1 Only the root process reads the input
     sscanf(argv[2], "%d", &n);
     
     // STEP - 2 Broadcast the integer number to all other MPI processess in the group
     MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
     
     //assign n amount of memory for primeNumbers array to be created
     primeNumbers = (int*)malloc(n * sizeof(int));
     
     //The number of elements each process should run
     elementsPerProcess = n / size;

     
     start = rank * elementsPerProcess; // starting element of each process
     end = start + elementsPerProcess;  //ending element of each proce
     
     // The last process gets the remaining elements
     if ( i == size -1){
     	end += (n % size);
     }
     
     
     //For the first process start checking prime numbers from 2
    if (start == 0) {
        start += 2;
    }
    
    // Get current clock time.
    clock_gettime(CLOCK_MONOTONIC, &startComp);
     
     // STEP 3 - Parallel computing takes place here
     for (i = start; i < end; i++) {
        
        if (isPrime(i)) {
            primeNumbers[primeCounter] = i;
            primeCounter++;
        }
    }
    
    // Get the clock current time again
    // Subtract end from start to get the CPU time used.
    clock_gettime(CLOCK_MONOTONIC, &endComp); 
    time_taken = (endComp.tv_sec - startComp.tv_sec) * 1e9; 
    time_taken = (time_taken + (endComp.tv_nsec - startComp.tv_nsec)) * 1e-9; 
    printf("Rank: %d. Overall time (product only)(s): %lf\n",rank, time_taken);
 
    // STEP 4 - Send the arrays results back to the root process
    if (rank == 0) {
        // Initialize the offset based on Rank 0's workload
        offset = primeCounter;
        
        for (i = 1; i < size; i++) {
            if (i != size - 1) {
                MPI_Recv(&primeCounter, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
		 MPI_Recv((int*)primeNumbers + offset, elementsPerProcess, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            } else {
                MPI_Recv(&primeCounter, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
		 MPI_Recv((int*)primeNumbers + offset, elementsPerProcess + (n%size), MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            }

            offset += primeCounter;
        }
        
        // STEP 5: Write to file
        printf("Rank: %d. Commence Writing\n", rank);
        writeToFile("process.txt", primeNumbers, offset);
        printf("Rank: %d. Write complete\n", rank);
        
        
    } else {

        MPI_Send(&primeCounter, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send((int*)primeNumbers, (end - start), MPI_INT, 0, 0, MPI_COMM_WORLD);
    } 
    
    free(primeNumbers);
    
    // Get the clock current time again
    // Subtract end from start to get the CPU time used.
    clock_gettime(CLOCK_MONOTONIC, &end_t); 
    time_taken = (end_t.tv_sec - start_t.tv_sec) * 1e9; 
    time_taken = (time_taken + (end_t.tv_nsec - start_t.tv_nsec)) * 1e-9; 
    printf("Rank: %d. Overall time (s): %lf\n",rank, time_taken);
    
    
    MPI_Finalize();
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

void writeToFile(char *pFilename, int *pArray, int counter) {
    int i;
    FILE *fp = fopen(pFilename, "w");
    for(i=0; i<counter;i++) {
        fprintf(fp, "%d\n", pArray[i]);
    }
    fclose(fp);
}
