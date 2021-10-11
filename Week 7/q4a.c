#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <unistd.h>

#define SIZE 100000000 // You can reduce this value based on available system memory

int main() {
    int my_rank;
    int p;
    int source;
    int dest;
    int tag = 0;
    int *pData;
    int i;
    
    //Initialize request array and counter 	
	MPI_Request req[4];
	int req_counter = 0;
	MPI_Status statusus[4]; //Initialize status array
    
    struct timespec start, end;
    double time_taken;
    int provided;
    
    MPI_Status status;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    
    pData = (int*)malloc(SIZE * sizeof(int));
    
    if(my_rank == 0)
    {
        srand(time(NULL));
        for(i = 0; i < SIZE; i++){
            pData[i] = rand() % 1500 + 1;
        }

        for(i = 1; i < p; i++){
            MPI_Isend(pData, SIZE, MPI_INT, i, i, MPI_COMM_WORLD, &req[req_counter++]);
        }
        MPI_Waitall( req_counter, req, statusus);
    } else {
        clock_gettime(CLOCK_MONOTONIC, &start);
        MPI_Recv(pData, SIZE, MPI_INT, 0, my_rank, MPI_COMM_WORLD, &status);
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_taken = (end.tv_sec - start.tv_sec) * 1e9;
        time_taken = (time_taken + (end.tv_nsec - start.tv_nsec))* 1e-9;
        printf("Rank: %d. Time required to receive data from root(s): %lf\n", my_rank, time_taken);
    }
    MPI_Finalize();
    free(pData);
    
    return 0;
}
