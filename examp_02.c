// source code of EXAMP_02.C program 
#include "mpi.h"
#include <stdio.h>

int main (int argc, char **argv)
{
 char message[20];
 int i=0, all_rank,my_rank; 
 MPI_Status status;
 MPI_Init (&argc, &argv);
 MPI_Comm_rank (MPI_COMM_WORLD, &my_rank); // get number current process
 MPI_Comm_size (MPI_COMM_WORLD,  &all_rank); // all process 

 if (my_rank==0)    /* code for process zero */
 {
  strcpy (message, "Hello, there !\0");
  for(i=0; i<all_rank; i++)  
   MPI_Send(message, strlen(message), MPI_CHAR, i, 999, MPI_COMM_WORLD);
 }
 else   /* code for other processes */
 {
  MPI_Recv(message, 20, MPI_CHAR, 0, 999, MPI_COMM_WORLD, &status);
//  MPI_Recv(message, 20, MPI_CHAR,  0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  printf ("I am %d process; received: %s\n", my_rank, message);
 }
  MPI_Finalize();
} // end of EXAMP_02 program
