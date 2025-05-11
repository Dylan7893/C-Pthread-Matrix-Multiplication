// Project3Question1.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Parallel program with phthreads to multiply two matrices filled with random values 0-9 
// 10/30/24
// Dylan Hardy

//include the time header function
#ifndef _TIMER_H_
#define _TIMER_H_

#include <sys/time.h>

// The argument now should be a double (not a pointer to a double) 
#define GET_TIME(now) { \
   struct timeval t; \
   gettimeofday(&t, NULL); \
   now = t.tv_sec + t.tv_usec/1000000.0; \
}

#endif

//include other appropriate headers
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

//global variables
const int MAX_THREADS = 1024; //constant that holds max number of threads
long      thread_count; //will hold number of threads being used
int       N, K, M; //rows and columns
int**     multiplierMatrix; //2d multiplier array 
int**     multiplicandMatrix; //2d multiplicand array
int**     productMatrix; //2d product array

//function prototypes
void* Multiply(void* thread); //function to multiply the matrices
void Get_args(int argc, char* argv[]); //function to get arguments
void Usage(char* prog_name); //prints errors
void DeallocateMatrix(int** matrix, int row); //fuction to deallocate matrix memory

int main(int argc, char* argv[])
{
    long   thread; //variable for rank
    double start_time, finish_time, execution_time; //get start/finish and calculated execution time

    //introduction and getting values of rows and columns
    printf("          Matrix Multiplication Program\n");
    printf("---------   N X K   *  K X M  =  N X M   ---------\n");
    printf("\nUsing the reference above enter a value for N: ");
    scanf("%d", &N);
    printf("\nUsing the reference above enter a value for K: ");
    scanf("%d", &K);
    printf("\nUsing the reference above enter a value for M: ");
    scanf("%d", &M);

    //allocate memory for all three matrices
    multiplierMatrix = (int**)malloc(sizeof(int*) * N); //memory for N X K matrix
    for (int index = 0; index < N; index++)
        multiplierMatrix[index] = (int*)malloc(sizeof(int) * K);

    multiplicandMatrix = (int**)malloc(sizeof(int*) * K); //memory for K X M matrix
    for (int index = 0; index < K; index++)
        multiplicandMatrix[index] = (int*)malloc(sizeof(int) * M);

    productMatrix = (int**)malloc(sizeof(int*) * N); //memory for N X M matrix
    for (int index = 0; index < N; index++)
        productMatrix[index] = (int*)malloc(sizeof(int) * M);

    srand(time(0)); //seed random number generator with the time

    //initialize the first matrix with random numbers 0-9
    for (int i = 0; i < N; i++) //iterate matrix rows
    {
        for (int j = 0; j < K; j++) //iterate matrix columns
        {
            multiplierMatrix[i][j] = rand() % 10; //initialize to random 0-9
        }
    }

    for (int i = 0; i < K; i++) //iterate matrix rows
    {
        for (int j = 0; j < M; j++) //iterate matrix columns
        {
            multiplicandMatrix[i][j] = rand() % 10; //initialize to random 0-9
        }
    }

    pthread_t* thread_handles; //create a pthread object
    Get_args(argc, argv); //get arguments

    thread_handles = (pthread_t*)malloc(thread_count * sizeof(pthread_t)); //allocate memory for the threads

    GET_TIME(start_time); //get the current time for start
    
    for (thread = 0; thread < thread_count; thread++) 
        pthread_create(&thread_handles[thread], NULL, Multiply, (void*)thread); //create threads and multiply matrices

    for (thread = 0; thread < thread_count; thread++)
        pthread_join(thread_handles[thread], NULL); //gathers threads when finished to sync

    GET_TIME(finish_time); //get the finish time

    //print results
    printf("\nProduct: \n"); //label
    for (int i = 0; i < N; i++) //iterates product matrix rows
    {
        for (int j = 0; j < M; j++) //iterates product matrix columns
        {
            printf("%d ", productMatrix[i][j]); //prints product matrix iterating through all rows/columns
        }
        printf("\n"); //after each row start new line
    }

    execution_time = finish_time - start_time; //calculate execution time
    printf("\nThe matrix was multiplied in %f seconds with %ld threads", execution_time, thread); //print results

    //free allocated memory
    DeallocateMatrix(multiplierMatrix, N);
    DeallocateMatrix(multiplicandMatrix, K);
    DeallocateMatrix(productMatrix, N);
    free(thread_handles);

    printf("\n"); //formatting
    return 0; //successful run
}

/*------------------------------------------------------------------
 * Function:   Multiply
 * Purpose:    multiply two matrices
 * In arg:     void pointer thread rank
 * Return val: void
 */
void* Multiply(void* thread)
{
    long rank = (long)thread; //sets local variable rank to paramater
    int local_n = N / thread_count; //calculates the rows per thread
    int remaining_rows = N % thread_count; //calculates remainder if odd amount
    int first_row; //where each thread will start
    int last_row; //where each thread will end

    first_row = rank * local_n; //finding start by multiplying the rank by row per thread 
    last_row = first_row + local_n - 1; //finding end by adding where the last ended minus one
    if (rank == thread_count - 1) //if there is a remainder
        last_row += remaining_rows; //add remainder at end 

    for (int i = first_row; i <= last_row; i++) //goes through rows N from defined start to finish
    {
        for (int j = 0; j < M; j++) //goes through columns M
        {
            productMatrix[i][j] = 0; //initialize each cell to zero
            for (int k = 0; k < K; k++) //goes through columns N and rows M
            {
                productMatrix[i][j] += multiplierMatrix[i][k] * multiplicandMatrix[k][j]; //multiply
            }
        }
    }

}

/*------------------------------------------------------------------
 * Function:   Get_args
 * Purpose:    gets and validates arguments
 * In arg:     number of arguments and actual command line arguments
 * Return val: void
 */
void Get_args(int argc, char* argv[])
{
    if (argc != 2) //expecting two arguments
        Usage(argv[0]); //error

    thread_count = strtol(argv[1], NULL, 10); //sets the thread count

    if (thread_count <= 0 || thread_count > MAX_THREADS) //validates thread count from 0-1024
        Usage(argv[0]); //error
}

/*------------------------------------------------------------------
 * Function:   DeallocateMatrix
 * Purpose:    deallocates the dynamic matrices
 * In arg:     dynamically allocated matrix and number of rows
 * Return val: void
 */
void DeallocateMatrix(int** matrix, int row)
{
    for (int i = 0; i < row; i++) //iterates through rows
    {
        free(matrix[i]); //deallocates rows
    }
    free(matrix); //deallocates array
}

/*------------------------------------------------------------------
 * Function:   Usage
 * Purpose:    prints errors for Get_args function
 * In arg:     program name
 * Return val: void
 */
void Usage(char* prog_name)
{
    fprintf(stderr, "usage: %s <num of threads>\n", prog_name);
    fprintf(stderr, "Error: Number of threads must be between 1 and %d.\n", MAX_THREADS);
    exit(0);
}