// source code of EXAMP_01.C program
#include "mpi.h"
#include <stdio.h> 
#include <sys/timeb.h> // for ftime function

int main(int argc, char **argv) 
{
 int CURR_PROC, ALL_PROC, NAME_LEN;
 char PROC_NAME[MPI_MAX_PROCESSOR_NAME];
 struct timeb t; // time contain structure
 
 MPI_Init (&argc, &argv);
 MPI_Comm_rank (MPI_COMM_WORLD, &CURR_PROC); /* current process */
 MPI_Comm_size (MPI_COMM_WORLD,  &ALL_PROC); /* all process */

// get processor name (really computer name)
 MPI_Get_processor_name(PROC_NAME, &NAME_LEN); 

 ftime(&t); // get current time point 
// t.time is time in sec since January 1,1970 
// t.millitm is part of time in msec
// ATTENSION ! ftime may by used only after synchronization all processors time

// output to STDIN
 printf("I am process %d from %d and my name is %s (time: %.3f sec)\r\n", 
          CURR_PROC, ALL_PROC, PROC_NAME, (t.time+1e-3*t.millitm));

 MPI_Finalize();
}  // end of EXAMPLE_01 program
