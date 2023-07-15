// source code of PROG_MPI.C program
#include "mpi.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

static int message[10000000]; // max length of message (in 2*bytes) 

int main (int argc, char *argv[])
{
 double  time_start,time_finish;
 int  i, n, nprocs, myid, len;
 MPI_Request rq;
 MPI_Status status;
 
 MPI_Init(&argc,&argv); /* initialization MPI */
 MPI_Comm_size(MPI_COMM_WORLD,&nprocs); /* all processes */
 MPI_Comm_rank(MPI_COMM_WORLD,&myid);  /* current process */
 
 printf("PROGMPI: N_Procs=%d My_Id=%d\n", nprocs, myid);
 fflush( stdout );
 len = atoi( argv[1] ); // get length message from command line
 printf( "length: %d byte\n", len );

 if (myid == 0) /* I a m MASTER-process ! */
 {
  n = 1000; /* number of cycles */
  MPI_Send(&n, 4, MPI_CHAR, nprocs-1, 99,MPI_COMM_WORLD);
  time_start=MPI_Wtime();
  for (i=1;i<=n;i++)
   {
    MPI_Send(message, len, MPI_CHAR, nprocs-1, 99, MPI_COMM_WORLD); /* in CHAR=bytes */
    MPI_Recv(message, len, MPI_CHAR, nprocs-1, 99, MPI_COMM_WORLD, &status);
   }
   time_finish=MPI_Wtime();
   printf( "\nTotal time = %g (sec)\nrate = %g (messages/sec)\ntime of message = %g (sec)\nspeed = %g (bytes/sec)\n\n", 
         1.0*(time_finish-time_start),     
	 (2.0*n/(time_finish-time_start)), // rate
	 1.0 / ((float)(2.0*n/(time_finish-time_start))), // 1.0 / rate
         (2.0*n*len/(time_finish-time_start)) ); 
  } /* end if (myid==0) */
  
  else 
  if (myid == (nprocs-1)) /* I am is last of all processes ! */
  {
   MPI_Recv(&n,4, MPI_CHAR, 0, 99, MPI_COMM_WORLD,&status);
   for (i=1;i<=n;i++)
    {
     MPI_Recv(message, len, MPI_CHAR, 0, 99, MPI_COMM_WORLD, &status);
     MPI_Send(message, len, MPI_CHAR, 0, 99, MPI_COMM_WORLD);
    }
  } /* end if (myid==(nproc-1)) */
 
 fflush( stdout );
 MPI_Finalize(); /* deinitialize MPI */
} // end of PROC_MPI program 
