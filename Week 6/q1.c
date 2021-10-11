#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv)
{
	int rank, s_value, r_value, size;
	MPI_Status status;
	MPI_Init( &argc, &argv );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	MPI_Comm_size( MPI_COMM_WORLD, &size );
	
	do {
		if (rank == 0) {
			printf("Enter a round number: ");
			fflush(stdout); //clear the output buffer and move the buffered data to console
			scanf( "%d", &s_value );
			
			// Add your code here
			//Performs a blocking send
			MPI_Send(&s_value, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
			
			//Performs a blocking receive
			MPI_Recv(&r_value, 1, MPI_INT, size - 1, 0, MPI_COMM_WORLD, &status);
			
			printf( "Process %d got %d from Process %d\n", rank, r_value, size - 1);
			fflush(stdout);
		}
		
		else {
			// Add your code here
			//Performs a blocking receive
			MPI_Recv(&r_value, 1, MPI_INT, rank -1, 0, MPI_COMM_WORLD, &status);
			
			if (rank < size -1)
				//Performs a blocking send
				MPI_Send(&r_value, 1, MPI_INT, rank +1, 0, MPI_COMM_WORLD);
				
			else
				MPI_Send(&r_value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
			
			printf( "Process %d got %d from Process %d\n", rank, r_value, rank - 1);
			fflush(stdout);
		}
	} while (r_value >= 0);
	
	MPI_Finalize( );
	return 0;
}
