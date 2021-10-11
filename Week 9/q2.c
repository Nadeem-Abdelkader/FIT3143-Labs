#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#define SHIFT_ROW 0
#define SHIFT_COL 1
#define DISP 1

int generatePrimeNumber();
bool isPrime(int n);


int main(int argc, char *argv[]) {

    int ndims=2, size, my_rank, reorder, my_cart_rank, ierr;
    int nrows, ncols;
    int nbr_i_lo, nbr_i_hi;
    int nbr_j_lo, nbr_j_hi;
    MPI_Comm comm2D;
    int dims[ndims],coord[ndims];
    int wrap_around[ndims];
    int number_of_directions = 4;

    /* start up initial MPI environment */
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* process command line arguments*/
    if (argc == 3) {
        nrows = atoi (argv[1]);
        ncols = atoi (argv[2]);
        dims[0] = nrows; /* number of rows */
        dims[1] = ncols; /* number of columns */
        if( (nrows*ncols) != size) {
            if( my_rank ==0) printf("ERROR: nrows*ncols)=%d * %d = %d != %d\n", nrows, ncols, nrows*ncols,size);
            MPI_Finalize();
            return 0;
        }
    } else {
        nrows=ncols=(int)sqrt(size);
        dims[0]=dims[1]=0;
    }

    /************************************************************
    */
    /* create cartesian topology for processes */
    /************************************************************
    */
    MPI_Dims_create(size, ndims, dims);

    if(my_rank==0)
        printf("Root Rank: %d. Comm Size: %d: Grid Dimension = [%d x %d] \n",my_rank,size,dims[0],dims[1]);
    
    /* create cartesian mapping */
    wrap_around[0] = wrap_around[1] = 0; /* periodic shift is false. */
    reorder = 1; // true
    
    ierr =0;
    ierr = MPI_Cart_create(MPI_COMM_WORLD, ndims, dims,wrap_around, reorder, &comm2D);
    if(ierr != 0) printf("ERROR[%d] creating CART\n",ierr);
    
    /* find my coordinates in the cartesian communicator group */
    MPI_Cart_coords(comm2D, my_rank, ndims, coord);
    
    /* use my cartesian coordinates to find my rank in cartesian group*/
    MPI_Cart_rank(comm2D, coord, &my_cart_rank);
    
    /* get my neighbors; axis is coordinate dimension of shift */
    /* axis=0 ==> shift along the rows: P[my_row-1]: P[me] :
    P[my_row+1] */
    /* axis=1 ==> shift along the columns P[my_col-1]: P[me] :
    P[my_col+1] */
    MPI_Cart_shift( comm2D, SHIFT_ROW, DISP, &nbr_i_lo, &nbr_i_hi);
    MPI_Cart_shift( comm2D, SHIFT_COL, DISP, &nbr_j_lo, &nbr_j_hi);

    
    MPI_Request send_request[number_of_directions];
    MPI_Request receive_request[number_of_directions];
    MPI_Status send_status[number_of_directions];
    MPI_Status receive_status[number_of_directions];
    
    //writes the name of each process file in string format
        char rankFile[50];
        sprintf(rankFile, "process_%d.txt", my_rank);
        
        //write the output elements of each process to a file
        FILE *fp = fopen(rankFile, "w");
    
    // Iterate through 5 times
    for (int j = 0; j < 5; j ++) {
        sleep(my_rank);
        
        // Random seed
        srand(time(NULL));
        
        // Generate random prime number
        int number = generatePrimeNumber();
        
        // The use of non-blocking communication
        MPI_Isend(&number, 1, MPI_INT, nbr_i_lo, 0, comm2D, &send_request[0]);
        MPI_Isend(&number, 1, MPI_INT, nbr_i_hi, 0, comm2D, &send_request[1]);
        MPI_Isend(&number, 1, MPI_INT, nbr_j_lo, 0, comm2D, &send_request[2]);
        MPI_Isend(&number, 1, MPI_INT, nbr_j_hi, 0, comm2D, &send_request[3]);
        
        // Initialize values to -1, where it indicates the direction does not have values
        int receive_value_l = -1, receive_value_r = -1, receive_value_t = -1, receive_value_b = -1;
        MPI_Irecv(&receive_value_t, 1, MPI_INT, nbr_i_lo, 0, comm2D, &receive_request[0]);
        MPI_Irecv(&receive_value_b, 1, MPI_INT, nbr_i_hi, 0, comm2D, &receive_request[1]);
        MPI_Irecv(&receive_value_l, 1, MPI_INT, nbr_j_lo, 0, comm2D, &receive_request[2]);
        MPI_Irecv(&receive_value_r, 1, MPI_INT, nbr_j_hi, 0, comm2D, &receive_request[3]);
        
        MPI_Waitall(number_of_directions, send_request, send_status);
        MPI_Waitall(number_of_directions, receive_request, receive_status);
        
        printf("Global rank: %d. Cart rank: %d. Coord: (%d, %d).Left: %d. Right: %d. Top: %d. Bottom: %d\n", my_rank, my_cart_rank, coord[0], coord[1], receive_value_l, receive_value_r, receive_value_t,receive_value_b);
        fflush(stdout);
        
        // Initialize check_match
        int check_match[number_of_directions];
        int i;
        
        for (i = 0; i < number_of_directions; i++) {
            check_match[i] = 0;
        }
        
        // Check on which direction there is a match
        if (number == receive_value_t)
            check_match[0] = number;
        if (number == receive_value_b)[1] = number;
        if (number == receive_value_l)
            check_match[2] = number;
            check_match
        if (number == receive_value_r)
            check_match[3] = number;
            
        // Write out the iteration number
        fprintf(fp, "Iteration %d\n", j+1);
        
        // Write out the matched numbers in its respective direction
        for (i = 0; i < number_of_directions; i++) {
            if (check_match[i] != 0) {
                if (i == 0)
                    fprintf(fp, "Top: %d\n", check_match[i]);
                else if (i == 1)
                    fprintf(fp, "Bottom: %d\n", check_match[i]);
                else if (i == 2)
                    fprintf(fp, "Left: %d\n", check_match[i]);
                else
                    fprintf(fp, "Right: %d\n", check_match[i]);
            } 
            
        }
        
        fprintf(fp, "\n");
    }
    fclose(fp);
    

    MPI_Comm_free( &comm2D );
    MPI_Finalize();
    return 0;
}

int generatePrimeNumber() {
    // Generating random numbers
    int random_number = (rand() % (20 - 2 + 1)) + 2;
    // Check if the generated random number is prime
    if (isPrime(random_number))
        return random_number;
    else
        // perform recursion if it is not a prime number
        return generatePrimeNumber();
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
