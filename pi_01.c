// source code PI_01.C program
#include "mpi.h"
#include <stdio.h>
#include <math.h>

#include <sys/timeb.h> // for ftime
double f_time(void); /* define real time by ftime function  */
#include "f_time.c"

double compute_interval (int myrank, int ntasks, long intervals);

int main(int argc, char ** argv) 
{
 long intervals=100; // number of integration’s parts
 int i, myrank,ranksize;
 double pi, di, send[2], recv[2],
             t1,t2, t3,t4, t5,t6,t7, // time’s moments
             pi_prec=4.0*(atanl(1.0)-atan(0.0)); // pi precision
 MPI_Status status;

 t1=f_time();		 
 MPI_Init (&argc, &argv);  /* initialize MPI-system */
 t2=f_time();
 MPI_Comm_rank (MPI_COMM_WORLD, &myrank); /* my place in MPI system */
 MPI_Comm_size (MPI_COMM_WORLD, &ranksize);  /* size of MPI system */

 if (myrank == 0)  /* I am the MASTER */
 {
 intervals = atoi( argv[1] ); /* get interval’s number from command line */
 printf ("Calculation of PI by numerical integration with %ld intervals\n", intervals);
 }

 MPI_Barrier(MPI_COMM_WORLD); /* make sure all MPI tasks are running */

 if (myrank == 0) /* I am the MASTER */
 {  /* distribute parameter */
 printf ("Master: Sending # of intervals to MPI-Processes \n");
 t3 = MPI_Wtime();
 for (i=1; i<ranksize; i++)  
  MPI_Send (&intervals, 1, MPI_LONG, i, 98, MPI_COMM_WORLD);
 } /* end work MASTER */
 else	/* I am a SLAVE */
 { /* receive parameters */
 MPI_Recv (&intervals, 1, MPI_LONG, 0, 98, MPI_COMM_WORLD, &status);
 } // end work SLAVE

 /* compute my portion of interval */
 t4 = MPI_Wtime();
 pi = compute_interval (myrank, ranksize, intervals); /*=======================*/
 t5 = MPI_Wtime();
 MPI_Barrier (MPI_COMM_WORLD); /* make sure all MPI tasks are running */
 t6 = MPI_Wtime();

 if (myrank == 0) /* I am the MASTER */
{  /* collect results add up & printing results */
 for (i=1; i<ranksize; i++)
 {
  MPI_Recv (&di, 1, MPI_DOUBLE, i, 99, MPI_COMM_WORLD, &status);
  pi += di;
 } /* end of collect results */
 t7 = MPI_Wtime();
 printf ("Master: Has collected sum from MPI-Processes \n");
 printf ("\nPi estimation: %.12lf (rel.error= %.5f %%)\n", 
           pi, 1.0e2*(pi_prec-pi)/pi_prec);
 printf ("%ld tasks used,  execution time: %.3lf sec\n",ranksize, t7 -t3);
 printf("\nStatistics:\n");
 printf("Master: startup: %.0lf msec\n",t2-t1);
 printf("Master: time to send # of intervals:%.3lf sec\n",t4-t3);
 printf("Master: waiting time for sincro after calculation:%.2lf sec\n",t6-t5);
 printf("Master: time to collect: %.3lf sec\n",t7-t6);
 printf("Master: calculation time:%.3lf sec\n",t5-t4);

 MPI_Barrier (MPI_COMM_WORLD); /* make sure all MPI tasks are running */

 for (i=1; i<ranksize; i++)
 {  /* collect there calculation time */
 MPI_Recv(recv, 2, MPI_DOUBLE, i, 100, MPI_COMM_WORLD, &status);
 printf("process %d: calculation time: %.3lf sec,\twaiting time for sincro.: %.3lf sec\n",        
           i,recv[0],recv[1]);
 } // end of collection
} // end work MASTER

 else /* I am a SLAVE */
{ /* send my result back to master */
 MPI_Send (&pi, 1, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD);
 MPI_Barrier (MPI_COMM_WORLD); /* make sure all MPI tasks are running */
 send[0]=t5-t4;
 send[1]=t6-t5;
 MPI_Send (send, 2, MPI_DOUBLE, 0, 100, MPI_COMM_WORLD);
} // end work SLAVE

 MPI_Finalize ();
} // end MAIN function

/* ============================================================== */
double compute_interval (int myrank, int ntasks, long intervals) 
{ /* calculate integral’s localsum */
 double x, width, localsum=0.0;
 long j;
 width = 1.0 / intervals; /* width of single stripe */
 for (j=myrank; j<intervals; j+= ntasks)
 {
  x = (j + 0.5) * width;
  localsum += 4 / (1 + x*x);
 }
 return (localsum * width);	/* area of stripe */
} // end of COMPUTE_INTERVAL function
// end of PI_01.C program
