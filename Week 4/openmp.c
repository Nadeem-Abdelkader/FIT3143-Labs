#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<omp.h>
// Defining constant values
#define NUMTHREADS 4
#define MAXIMUM 25
#define MINIMUM 1
#define SIZE 10

// A list that holds the random number
int list[SIZE];
// A list to find the occurrence of the random number
int occurrence[MAXIMUM];

int main()
{

    // Random seed
    srand(time(NULL));

    int i;
    // Initialize the counter of number of wins
    int counter = 0;
    
    // Initialize occurrence list into list of 0
    for (i = 0; i< MAXIMUM; i++) {
        occurrence[i] = 0;
    }
    
    // Setting the number of threads to be NUMTHREADS
    omp_set_num_threads(NUMTHREADS);

    /*
     * Generating random numbers and assigned into list 
     * Update the occurrence list
     */
    #pragma omp parallel for private(i) shared(list, occurrence) schedule(static, 2)
    for(i = 0; i < SIZE; i++) {
        list[i] = rand() % MAXIMUM + MINIMUM;
        occurrence[list[i]-1]++;
    }
    
    // Printing values in the list
    for(i = 0; i < SIZE; i++) {
        printf("list[%d]: %d\n", i, list[i]);
    }
    
    // Count the number of wins and printing the number and the number of occurrence if and only if the occurrence is more than 1
    for(i = 0; i < MAXIMUM; i++) {
        if (occurrence[i] > 1) {
            counter++;
            printf("Number %d appears %d time(s)\n", i+1, occurrence[i]);
        }
    }
    
    // Printing the number of wins
    printf("The total number of wins: %d\n", counter);
    
    return 0;
}
