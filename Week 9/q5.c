#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

// Define shift_height constant
#define SHIFT_HEI 0
#define SHIFT_ROW 1
#define SHIFT_COL 2

#define DISP 1

int generatePrimeNumber();
bool isPrime(int n);


int main(int argc, char *argv[]) {

    // number of dimension is 3
    int ndims=3, size, my_rank, reorder, my_cart_rank, ierr;
    int nrows, ncols, nheight;
    int nbr_i_lo, nbr_i_hi;
    int nbr_j_lo, nbr_j_hi;
    int nbr_k_lo, nbr_k_hi;
    MPI_Comm comm3D;
    int dims[ndims],coord[ndims];
    int wrap_around[ndims];
    int number_of_directions = 6;

    /* start up initial MPI environment */
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* process command line arguments*/
    if (argc == 4) {
        nrows = atoi (argv[1]);
        ncols = atoi (argv[2]);
        nheight = atoi (argv[3]);
        dims[0] = nrows; /* number of rows */
        dims[1] = ncols; /* number of columns */
        dims[2] = nheight; /* height for the dimension */
        
        if( (nrows*ncols*nheight) != size) {
            if( my_rank ==0) printf("ERROR: nrows*ncols*nheight)=%d * %d *%d = %d != %d\n", nrows, ncols, nheight, nrows*ncols*nheight, size);
            MPI_Finalize();
            return 0;
        }
    } else {
        nrows=ncols=nheight=(int)cbrt(size);
        dims[0]=dims[1]=dims[2]=0;
    }

    /************************************************************
    */
    /* create cartesian topology for processes */
    /************************************************************
    */
    MPI_Dims_create(size, ndims, dims);

    if(my_rank==0)
        printf("Root Rank: %d. Comm Size: %d: Grid Dimension = [%d x %d x %d] \n",my_rank,size,dims[0],dims[1],dims[2]);
    
    /* create cartesian mapping */
    wrap_around[0] = wrap_around[1] = wrap_around[2] = 0; /* periodic shift is false. */
    reorder = 1; // true
    
    ierr =0;
    ierr = MPI_Cart_create(MPI_COMM_WORLD, ndims, dims,wrap_around, reorder, &comm3D);
    if(ierr != 0) printf("ERROR[%d] creating CART\n",ierr);
    
    /* find my coordinates in the cartesian communicator group */
    MPI_Cart_coords(comm3D, my_rank, ndims, coord);
    
    /* use my cartesian coordinates to find my rank in cartesian group*/
    MPI_Cart_rank(comm3D, coord, &my_cart_rank);
    
    /* get my neighbors; axis is coordinate dimension of shift */
    /* axis=0 ==> shift along the rows: P[my_row-1]: P[me] :
    P[my_row+1] */
    /* axis=1 ==> shift along the columns P[my_col-1]: P[me] :
    P[my_col+1] */
    
    MPI_Cart_shift( comm3D, SHIFT_ROW, DISP, &nbr_i_lo, &nbr_i_hi);
    MPI_Cart_shift( comm3D, SHIFT_COL, DISP, &nbr_j_lo, &nbr_j_hi);
    MPI_Cart_shift( comm3D, SHIFT_HEI, DISP, &nbr_k_lo, &nbr_k_hi);
    
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
        // To ensure that the random value does not repeat
        sleep(my_rank);
        
        // Random seed
        srand(time(NULL));
        
        // Generate random prime number
        int number = generatePrimeNumber();
        
        // The use of non-blocking communication
        MPI_Isend(&number, 1, MPI_INT, nbr_i_lo, 0, comm3D, &send_request[0]);
        MPI_Isend(&number, 1, MPI_INT, nbr_i_hi, 0, comm3D, &send_request[1]);
        MPI_Isend(&number, 1, MPI_INT, nbr_j_lo, 0, comm3D, &send_request[2]);
        MPI_Isend(&number, 1, MPI_INT, nbr_j_hi, 0, comm3D, &send_request[3]);
        MPI_Isend(&number, 1, MPI_INT, nbr_k_hi, 0, comm3D, &send_request[4]);
        MPI_Isend(&number, 1, MPI_INT, nbr_k_lo, 0, comm3D, &send_request[5]);
        
        // Initialize values to -1, where it indicates the direction does not have values
        int receive_value_le = -1, receive_value_ri = -1, receive_value_to = -1, receive_value_bo = -1, receive_value_fr = -1, receive_value_re = -1;
        
        MPI_Irecv(&receive_value_to, 1, MPI_INT, nbr_i_lo, 0, comm3D, &receive_request[0]);
        MPI_Irecv(&receive_value_bo, 1, MPI_INT, nbr_i_hi, 0, comm3D, &receive_request[1]);
        MPI_Irecv(&receive_value_le, 1, MPI_INT, nbr_j_lo, 0, comm3D, &receive_request[2]);
        MPI_Irecv(&receive_value_ri, 1, MPI_INT, nbr_j_hi, 0, comm3D, &receive_request[3]);
        MPI_Irecv(&receive_value_re, 1, MPI_INT, nbr_k_hi, 0, comm3D, &receive_request[4]);
        MPI_Irecv(&receive_value_fr, 1, MPI_INT, nbr_k_lo, 0, comm3D, &receive_request[5]); 
        
        MPI_Waitall(number_of_directions, send_request, send_status);
        MPI_Waitall(number_of_directions, receive_request, receive_status);
        
        printf("Global rank: %d. Cart rank: %d. Coord: (%d, %d, %d).Left: %d. Right: %d. Top: %d. Bottom: %d. Rear: %d. Front: %d\n", my_rank, number, coord[0], coord[1], coord[2], receive_value_le, receive_value_ri, receive_value_to,receive_value_bo, receive_value_re, receive_value_fr);
        fflush(stdout);
        
        // Initialize check_match
        int check_match[number_of_directions];
        // Initialize i
        int i;
        
        // Initialize values to check_match
        for (i = 0; i < number_of_directions; i++) {
            check_match[i] = 0;
        }
        
        // Check on which direction there is a match
        if (number == receive_value_to)
            check_match[0] = number;
        if (number == receive_value_bo)
            check_match[1] = number;
        if (number == receive_value_le)
            check_match[2] = number;
        if (number == receive_value_ri)
            check_match[3] = number;
        if (number == receive_value_re)
            check_match[4] = number;
        if (number == receive_value_fr)
            check_match[5] = number;
            
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
                else if (i == 3)
                    fprintf(fp, "Right: %d\n", check_match[i]);
                else if (i == 4)
                    fprintf(fp, "Rear: %d\n", check_match[i]);
                else
                    fprintf(fp, "Front: %d\n", check_match[i]);
            } 
            
        }
        
        // Write out new line
        fprintf(fp, "\n");
    }
    fclose(fp);
    

    MPI_Comm_free( &comm3D );
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
