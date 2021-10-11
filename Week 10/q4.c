#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
	FILE *pInfile;
	float a_coeff, b_coeff, c_coeff, x1, x2, disc;
	float x1r, x1i, x2r, x2i;
	int fileElementCount = 0, constNeg = -1;
	int counter = 0;
	
	char rankFile[50];
	
	int my_rank;
	int p;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	
	// WRITE PART(a) HERE
	if (my_rank == 0){
		pInfile = fopen("quad.txt","r");
		fscanf(pInfile, "%d", &fileElementCount);

	}
	// Broadcast the counter to all process
	MPI_Bcast(&fileElementCount,1,MPI_INT,0,MPI_COMM_WORLD);
	
	switch (my_rank){ 
		case 0:{	
			//read second line for a, b and c before reading the quadratic values
			char a,b,c;
			fscanf(pInfile, "%s %s %s", &a,&b,&c);
			
			// Read each element from the file
			while(counter < fileElementCount)
			{
				fscanf(pInfile, "%f %f %f", &a_coeff, &b_coeff, &c_coeff);
				
				disc = pow(b_coeff,2) - (4*a_coeff*c_coeff); // calculate the discriminant
				
				//Sending the discriminant and the coefficient values to next node
				MPI_Send(&disc, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
				MPI_Send(&a_coeff, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
				MPI_Send(&b_coeff, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
				MPI_Send(&c_coeff, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
				counter++;
			}

			fclose(pInfile);
			pInfile = NULL;
			
			break;
		}
		case 1:{
		
			// WRITE PART (b) HERE 
			do
			{ 
				//Receive the discriminant and the coefficient values from previous node
				MPI_Recv(&disc, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &status);
				MPI_Recv(&a_coeff, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &status);
				MPI_Recv(&b_coeff, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &status);
				MPI_Recv(&c_coeff, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &status);				
				
				if (disc >0 || disc == 0){
					//has two roots
					if (disc > 0){
						x1 = (-b_coeff + sqrt(disc)) / (2 * a_coeff);
						x2 = (-b_coeff - sqrt(disc)) / (2 * a_coeff);
					}
					//has one real root
					else if (disc == 0){
						x1 = (-b_coeff) / (2 * a_coeff);
						x2 = x1;
					}
					
					//Send two roots or one real root to next node
					MPI_Send(&x1, 1, MPI_FLOAT, 2, 0, MPI_COMM_WORLD);
					MPI_Send(&x2, 1, MPI_FLOAT, 2, 0, MPI_COMM_WORLD);
				}
				//discriminant < 0
				else{
					//has two distinct complex roots
					x1r = (-b_coeff) / (2 * a_coeff);
					x1i = sqrt(abs(disc)) / (2 * a_coeff);
					x2r = x1r;
					x2i = -x1i;
					
					//Send the complex roots to next node
					MPI_Send(&x1r, 1, MPI_FLOAT, 2, 0, MPI_COMM_WORLD);
					MPI_Send(&x1i, 1, MPI_FLOAT, 2, 0, MPI_COMM_WORLD);
					MPI_Send(&x2r, 1, MPI_FLOAT, 2, 0, MPI_COMM_WORLD);
					MPI_Send(&x2i, 1, MPI_FLOAT, 2, 0, MPI_COMM_WORLD);
				}
				MPI_Send(&disc, 1, MPI_FLOAT, 2, 0, MPI_COMM_WORLD);
				counter++;
				
			}while(counter < fileElementCount);
			break;
		}
		case 2:{
			//write the name of the file
			sprintf(rankFile, "roots.txt");
			FILE *fp = fopen(rankFile, "w");
			
			//write the first and second line of file
			fprintf(fp, "%d\n", fileElementCount);

			fprintf(fp, "  x1       x2       x1_real       x1_img       x2_real       x2_img\n");
			do
			{
				MPI_Recv(&disc, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD, &status);
				//printf("disc: %f\n", disc);
				if (disc >0 || disc == 0){
					
					//Receive two roots or one real root from previous node
					MPI_Recv(&x1, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD, &status);
					MPI_Recv(&x2, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD, &status);

					fprintf(fp, "%7.1f %7.1f\n", x1, x2); //write output elements to file
				}
				else{
					
					////Receive complex roots from previous node
					MPI_Recv(&x1r, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD, &status);
					MPI_Recv(&x1i, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD, &status);
					MPI_Recv(&x2r, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD, &status);
					MPI_Recv(&x2i, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD, &status);
					fprintf(fp, "                  %7.1f%14.1f%14.1f%14.1f\n", x1r, x1i, x2r, x2i ); //write output elements to file
					
				}
				counter++;
				
			}while(counter < fileElementCount);
			fclose(fp);
			
			break;
		}

	}
	MPI_Finalize();
	return 0;
}