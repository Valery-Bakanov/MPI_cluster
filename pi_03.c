// source code PI_03.C program 
#include "mpi.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define DARTS    10000  /* number of throws at dartboard */
#define ROUNDS 100     /* number of times ‘darts’ is iterated */
#define MASTER   0        /* task ID of MASTER task */

void srandom (unsigned seed);  
double dboard (int darts);
#include "dboard.c" // including dboard.c file

int main(int argc, char *argv[])
{
double homepi, pisum, pi, avepi,
            t1, t2, // time’s moments
            pi_prec=4.0*(atanl(1.0)-atan(0.0)); // pi precision
int	 taskid, numtasks, rc, i;
MPI_Status status;

MPI_Init(&argc,&argv);
MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
MPI_Comm_rank(MPI_COMM_WORLD,&taskid);

t1=MPI_Wtime(); // fix start time calculated

avepi = 0;
for (i=0; i<ROUNDS; i++)
{
 homepi = dboard(DARTS);
 rc = MPI_Reduce(&homepi, &pisum, 1, MPI_DOUBLE, MPI_SUM, MASTER,  
                             MPI_COMM_WORLD);
 if (rc != MPI_SUCCESS)
  printf("%d: failure on MPI_Reduce\n", taskid);

if (taskid == MASTER)
{
  pi = pisum/numtasks;
  avepi = ((avepi * i) + pi)/(i + 1); 
  t2=MPI_Wtime(); // store end time calculated
  printf("%9d throws aver.value of PI= %.12lf (rel.error= %.5lf %%, time= %.3lf sec)\n",  
           (DARTS * (i+1)), avepi, 1.0e2*(pi_prec-avepi)/pi_prec, t2-t1);
}    
} // end for (i=0; i<ROUNDS; i++)
 MPI_Finalize();
 return 0;
} // end of MAIN function
// end of PI_03 program
