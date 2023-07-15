// source code of MM-MPIM-STAT.C program

#include "mpi.h"
#include <stdio.h>

#define NRA 1000 /* number of rows in matrix [A] and [C] */
#define NCA 1000 /* number of columns in matrix [A] and rows in [B] */
#define NCB 1000 /* number of columns in matrix [B] and [C] */

#define min(a,b) (((a)<(b))?(a):(b)) // min (a,b)

#define MASTER 0      /* taskid of first task */
#define FROM_MASTER 7 /* setting a message type */
#define FROM_WORKER 8 /* setting a message type */

int main(int argc, char *argv[])
{
int numtasks,              /* number of tasks in partition */
    taskid,                /* a task identifier */
    numworkers,            /* number of worker tasks */
    source,                /* task id of message source */
    dest,                  /* task id of message destination */
    rows,                  /* rows of matrix A sent to each worker */
    averow, extra, offset, /* used to determine rows sent to each worker */
   i,j,k,rc;               /* misc */
REAL sum_diag = 0.0; // summa of diagonal elements
static
REAL a[NRA][NCA],           /* matrix A to be multiplied */
     b[NCA][NCB],           /* matrix B to be multiplied */
     c[NRA][NCB];           /* result matrix C */
double
    t1,t2; // time's momemts
MPI_Status status;

   rc = MPI_Init(&argc,&argv);
   rc|= MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
   rc|= MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
   if (rc != MPI_SUCCESS)
    printf ("error initializing MPI and obtaining task ID information\n");
   else
    printf ("task ID = %d\n", taskid);
   numworkers = numtasks-1;
/**************************** master task ************************************/
   if (taskid == MASTER)
   {
    printf("\nNumber of worker tasks = %d\n",numworkers);

    for (i=0; i<NRA; i++)
     for (j=0; j<NCA; j++)
      a[i][j] = i+j;

    for (i=0; i<NCA; i++)
     for (j=0; j<NCB; j++)
      b[i][j] = i*j;

/* send matrix data to the worker tasks */
      averow = NRA / numworkers;
      extra = NRA % numworkers;
      offset = 0;

t1=MPI_Wtime(); // get start time's moments

      for (dest=1; dest<=numworkers; dest++)
      {
         rows = (dest <= extra) ? averow+1 : averow;   	
//         printf("...sending %d rows to task %d\n",rows,dest);
         MPI_Send(&offset, 1, MPI_INT, dest, FROM_MASTER, MPI_COMM_WORLD);
         MPI_Send(&rows,   1, MPI_INT, dest, FROM_MASTER, MPI_COMM_WORLD);
         MPI_Send(&a[offset][0], rows*NCA, MPI_DOUBLE, dest, FROM_MASTER,
                                                       MPI_COMM_WORLD);
         MPI_Send(&b, NCA*NCB, MPI_DOUBLE, dest, FROM_MASTER, MPI_COMM_WORLD);
         offset += rows;
      }

/* wait for results from all worker tasks */
      for (i=1; i<=numworkers; i++)
      {
        MPI_Recv(&offset, 1, MPI_INT, i, FROM_WORKER, MPI_COMM_WORLD, &status);
        MPI_Recv(&rows,   1, MPI_INT, i, FROM_WORKER, MPI_COMM_WORLD, &status);
        MPI_Recv(&c[offset][0], rows*NCB, MPI_DOUBLE, i, FROM_WORKER, MPI_COMM_WORLD, &status);
      }

t2=MPI_Wtime(); // get end time's momemts

     for (i=0; i<min(NRA,NCB); i++) // calculate of matrix [C] trace
      sum_diag += c[i][i];

printf ("\nMultiply time = %.3lf sec\n\nResult matrix trace = %.4le\n", t2-t1, sum_diag);

/* print results... it's not needs!!!

  printf("It's result matrix:\n");
  for (i=0; i<NRA; i++)
   {
    printf("\n"); 
    for (j=0; j<NCB; j++) 
     printf("%8.2f   ", c[i][j]);
   }
   printf ("\n");
*/

} // end MASTER work

/**************************** worker task ************************************/
   if (taskid > MASTER)
   {
    MPI_Recv(&offset, 1, MPI_INT, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
    MPI_Recv(&rows,   1, MPI_INT, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
    MPI_Recv(&a, rows*NCA, MPI_DOUBLE, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
    MPI_Recv(&b, NCA*NCB,  MPI_DOUBLE, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);

    for (k=0; k<NCB; k++)
     for (i=0; i<rows; i++)
      {
       c[i][k] = 0.0;
       for (j=0; j<NCA; j++)
        c[i][k] += a[i][j] * b[j][k];
      }

    MPI_Send(&offset, 1, MPI_INT, MASTER, FROM_WORKER, MPI_COMM_WORLD);
    MPI_Send(&rows,   1, MPI_INT, MASTER, FROM_WORKER, MPI_COMM_WORLD);
    MPI_Send(&c, rows*NCB, MPI_DOUBLE, MASTER, FROM_WORKER, MPI_COMM_WORLD);

   } // end if (taskid...)

   MPI_Finalize();

} // end of program
