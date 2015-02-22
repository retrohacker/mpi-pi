/*
 * File:    pi.c
 *
 * Purpose: Estimate pi using a monte carlo method
 *
 * Compile: mpicc -o pi.c monte -lm
 *
 * Run:     mpiexec -n <comm_sz> ./pi
 *
 * Input:   Number of "dart tosses"
 * Output:  Estimate of pi.
 *
 * Author:  Jinyoung Choi
 *          William Blankenship
 *
 * IPP:     Programming Assignment 3.2
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

void e(int error);
void Get_input(unsigned long long int* number_of_tosses, int my_rank, MPI_Comm comm);
unsigned long long int Monte_carlo(unsigned long long int number_of_tosses, int my_rank);

int main(int argc, char *argv[]) {
  unsigned long long int number_of_tosses;
  unsigned long long int local_number_of_tosses;
  unsigned long long int number_in_circle;
  unsigned long long int local_number_in_circle;

  double pi_estimate;
  int my_rank;
  int comm_sz;
  MPI_Comm comm;

  // Initialize the MPI environment
  e(MPI_Init(&argc, &argv));
  comm = MPI_COMM_WORLD;
  e(MPI_Comm_size(comm, &comm_sz));
  e(MPI_Comm_rank(comm, &my_rank));

  // Call Get_input function to get the number_of_tosses from user
  Get_input(&number_of_tosses, my_rank, comm);

  // Get the number of tosses that should be conducted on each node
  local_number_of_tosses = number_of_tosses/comm_sz;
  number_of_tosses = local_number_of_tosses * comm_sz;
  local_number_in_circle = Monte_carlo(local_number_of_tosses,my_rank);
  MPI_Reduce(&local_number_in_circle,&number_in_circle,1,MPI_UNSIGNED_LONG_LONG,MPI_SUM,0,comm);
  if(my_rank == 0) {
    pi_estimate = 4 * ((double) number_in_circle) / ((double) number_of_tosses);
    printf("Number of Tosses: %llu\nNumber in Circle: %llu\nPi is approx: %f\n",number_of_tosses,number_in_circle,pi_estimate);
  }

  MPI_Finalize();
  return 0;
}  /* main */

/* Handle all errors returned from MPI_* function calls */
void e(int error) {
  if(error != MPI_SUCCESS) {
    fprintf(stderr,"Error starting MPI program, Terminating.\n");
    MPI_Abort(MPI_COMM_WORLD,error);
    MPI_Finalize();
    exit(1);
  }
}


/* Get the number_of_tosses from user */
void Get_input( unsigned long long int* number_of_tosses  /* out */,
                int my_rank                               /* in  */,
                MPI_Comm comm                             /* in  */) {
  if(my_rank == 0) {
    printf("Count: ");
    scanf("%llu",number_of_tosses);
  }
  e(MPI_Bcast(number_of_tosses,1,MPI_UNSIGNED_LONG_LONG,0,comm));
}  /* Get_input */


/* Compute the number_in_circle out of the local_number_of_tosses on each node */
unsigned long long int Monte_carlo(unsigned long long int local_number_of_tosses, int my_rank) {
  unsigned long long int result = 0;
  double radius = 1;
  unsigned long long int i;
  for(i = 0; i < local_number_of_tosses; i++) {
    // We only sample from quadrant II, yields same result and simplifies logic
    double x = ((double) rand()) / ((double) RAND_MAX) * radius * 2;
    double y = ((double) rand()) / ((double) RAND_MAX) * radius * 2;
    int dist = sqrt(x * x + y * y);
    if(dist <= 1) result ++;
  }
  return result;
}  /* Monte_carlo */
