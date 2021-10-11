#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

//To be used as special MPI tags 
#define ORDERED 1
#define UNORDERED 2
#define EXIT 0

//Dividing the processes in MPI_COMM_WORLD into two sets 
//with 1 process as master and the rest are slaves
int master_io(MPI_Comm master_comm, MPI_Comm comm);
int slave_io(MPI_Comm master_comm, MPI_Comm comm);

int main(int argc, char **argv)
{
    int rank;
    MPI_Comm new_comm;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    //Make process 0 as the master process
    MPI_Comm_split( MPI_COMM_WORLD,rank == 0, 0, &new_comm);
    if (rank == 0) 
	master_io( MPI_COMM_WORLD, new_comm );
    else
	slave_io( MPI_COMM_WORLD, new_comm );
    MPI_Finalize();
    return 0;
}

/* This is the master */
int master_io(MPI_Comm master_comm, MPI_Comm comm)
{
    int i,j, size;
    char buf[256], buf2[256];
    MPI_Status status;
    MPI_Comm_size( master_comm, &size );
    
    int slaves = size - 1;
    int firstmsg;
    while (slaves > 0){
    	MPI_Recv( buf , 256 , MPI_CHAR , MPI_ANY_SOURCE , MPI_ANY_TAG ,master_comm , &status) ;
    	
	//if the tag is ORDERED
	//This allows us to print out any ordered messages first
    	if (status.MPI_TAG == ORDERED) {
    		firstmsg = status.MPI_SOURCE;
		for (i=1; i<size; i++) {
			if (i == firstmsg)
				fputs( buf, stdout );
			
			else{
		    		MPI_Recv( buf2, 256, MPI_CHAR, i, ORDERED, master_comm, &status );
		    		fputs( buf2, stdout );
			}
		}
    	}
    	
	//if the tag is UNORDERED
    	else if (status.MPI_TAG == UNORDERED){
    		fputs( buf, stdout );
    	}
	//if the tag is EXIT
	//exit each slaves that are done 
    	else if (status.MPI_TAG == EXIT){
    		slaves -=1;
    	}
 
    }
    
    return 0;
}

/* This is the slave */
int slave_io(MPI_Comm master_comm, MPI_Comm comm)
{
    char buf[256];
    int  rank;
    //send ordered message
    MPI_Comm_rank(comm, &rank);
    sprintf(buf, "Hello from slave %d\n", rank);
    MPI_Send( buf, strlen(buf) + 1, MPI_CHAR, 0, ORDERED, master_comm ); // master_comm = MPI_COMM_WORLD
    
    //send ordered message
    sprintf( buf, "Goodbye from slave %d\n", rank );
    MPI_Send( buf, strlen(buf) + 1, MPI_CHAR, 0, ORDERED, master_comm ); // master_comm = MPI_COMM_WORLD

    //send unordered message    
    sprintf( buf, "I'm exiting %d\n", rank );
    MPI_Send( buf, strlen(buf) + 1, MPI_CHAR, 0, UNORDERED, master_comm ); // master_comm = MPI_COMM_WORLD

    //send exit message to end the communication
    MPI_Send(buf, 0, MPI_CHAR, 0, EXIT, master_comm); 
    return 0;
}