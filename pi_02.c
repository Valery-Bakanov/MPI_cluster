// source code PI_02.C program
#include "mpi.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define DARTS    10000  /* number of throws at dartboard */
#define ROUNDS 100     /* number of times ‘darts’ is iterated */
#define MASTER   0       /* task ID of master task */

void srandom (unsigned seed);  
double dboard (int darts);
#include "dboard.c" // including dboard.c file

int main(int argc, char *argv[])
{
double homepi,   /* value of pi calculated by current task */
	pi,             /* average of pi after ‘darts’ is thrown */
	avepi,       /* average pi value for all iterations */
	pirecv,      /* pi received from worker */
	pisum,      /* sum of workers PI values */
           t1, t2, // time’s moments
           pi_prec=4.0*(atanl(1.0)-atan(0.0)); // pi precision
int	taskid,       /* task ID - also used as seed number */
	numtasks, /* number of tasks */
	source,      /* source of incoming message */ 
	mtype,      /* message type */
	rc,             /* MPI’s return code */
	i, n;
MPI_Status status;

rc = MPI_Init(&argc,&argv);
rc = MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
rc = MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
	
t1=MPI_Wtime(); // store start time calculated

srandom (taskid); // initialization random number generator by taskid value

avepi = 0;
for (i=0; i<ROUNDS; i++)
{
 homepi = dboard(DARTS); // all tasks calculate pi by dartboard algorithm 

if (taskid != MASTER) // It’s NOT MASTER!
{
 mtype = i;
 rc = MPI_Send(&homepi, 1, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD);
 if (rc != MPI_SUCCESS) // if error MPI_Send function
  printf("%d: Send failure on round %d\n", taskid, mtype);
} 
else // I am MASTER
{
 mtype = i;
 pisum = 0;
 for (n=1; n<numtasks; n++)
 {
  rc = MPI_Recv(&pirecv, 1, MPI_DOUBLE, MPI_ANY_SOURCE, mtype, 
                           MPI_COMM_WORLD, &status);
  if (rc != MPI_SUCCESS) // if error MPI_Recv function
    printf("%d: Receive failure on round %d\n", taskid, mtype);
  pisum = pisum + pirecv;
 }
/* Master calculates the average value of pi for this iteration */
 pi = (pisum + homepi)/numtasks;
/* Master calculates the average value of pi over all iterations */
 avepi = ((avepi * i) + pi) / (i + 1); 
 t2=MPI_Wtime(); // fix end time calculated
 printf("%9d throws aver.value of PI= %.12lf (rel.error= %.5lf %%, time= %.3lf sec)\n",  
         (DARTS * (i+1)), avepi, 1.0e2*(pi_prec-avepi)/pi_prec, t2-t1);
  }    
} // end for (i=0; i<ROUNDS; i++)
 MPI_Finalize();
 return 0;
} // end of MAIN function
// end of PI_02.C program
