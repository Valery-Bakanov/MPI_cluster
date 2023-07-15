// source code of MM-MPI_M-DYN.C program

#include "mpi.h"
#include <stdio.h>

int NRA = 1000, /* number of rows in matrix [A] and [C] */
    NCB = 1000, /* number of columns in matrix [B] and [C] */
    NCA = 1000; /* number of columns in matrix [A] and rows [B] */

#define REAL double /* type of REAL value */

REAL *pA, *pB, *pC; /* pointers on [A], [B] and [C] matrix */

#define A(i,k) pA[i*NCA+k] /* macros for adressing [A] matrix */
#define B(k,j) pB[k*NCB+j] /* macros for adressing [B] matrix */
#define C(i,j) pC[i*NCB+j] /* macros for adressing [C] matrix */

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
     rows,                  /* rows of matrix [A] sent to each worker */
     averow, extra, offset, /* used to determine rows sent to each worker */
     rc; /* errorcode */
 register
 int i,j,k; /* indexes */
 REAL sum_diag = 0.0; /* summa of diagonal elements */
 double t1,t2; /* time moments */

 MPI_Status status;

 rc = MPI_Init(&argc,&argv); /* get arguments of MPI-system */
 rc|= MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
 rc|= MPI_Comm_rank(MPI_COMM_WORLD,&taskid);

 if (rc != MPI_SUCCESS)
  printf ("...error initializing MPI and obtaining task ID information\n");
 else
  printf ("task ID = %d\n", taskid);

/*****************************************************************************/

 NRA = atoi(argv[1]); /* number of rows in matrix [A] and [C] */
 NCB = atoi(argv[2]); /* number of columns in matrix [B] and [C] */
 NCA = atoi(argv[3]); /* number of columns in matrix [A] and row */
 
 if (taskid == MASTER)
  printf("\nOrder of multiplay matrix:\nNRA = %d\nNCB = %d\nNCA = %d\n", 
           NRA,NCB,NCA); 

 pA = (REAL*) calloc(NRA*NCA, sizeof(REAL)); 
 pB = (REAL*) calloc(NCA*NCB, sizeof(REAL)); 
 pC = (REAL*) calloc(NRA*NCB, sizeof(REAL));  /* allocate and zeroing memory  */

 if(!pA || !pB || !pC) /* if return value = NULL... */
 {
  printf("\n\nOut of memory for pA, pB or pC...\n\n");
  exit(-13);
 }

 numworkers = numtasks-1;

/**************************** master task ************************************/

 if (taskid == MASTER)
 {
  printf("\nCount of worker tasks = %d\n", numworkers);

  for (i=0; i<NRA; i++)
   for (j=0; j<NCA; j++)
    A(i,j) = i+j; 

  for (i=0; i<NCA; i++)
   for (j=0; j<NCB; j++)
    B(i,j) = i*j; 
    
/* send matrix data to the worker tasks */
  averow = NRA / numworkers;
  extra = NRA % numworkers;
  offset = 0;

 t1 = MPI_Wtime(); // let start time's moments

 for (dest=1; dest<=numworkers; dest++)
 {
  rows = (dest <= extra) ? averow+1 : averow;   	
/* printf("...sending %d rows to task %d\n",rows,dest); */
  MPI_Send(&offset, 1, MPI_INT, dest, FROM_MASTER, MPI_COMM_WORLD);
  MPI_Send(&rows,   1, MPI_INT, dest, FROM_MASTER, MPI_COMM_WORLD);
  MPI_Send(&A(offset,0), rows*NCA, MPI_DOUBLE, dest, FROM_MASTER, MPI_COMM_WORLD);
  MPI_Send(&B(0,0), NCA*NCB, MPI_DOUBLE, dest, FROM_MASTER, MPI_COMM_WORLD);
  offset += rows;
 }

/* wait for results from all worker tasks */
 for (i=1; i<=numworkers; i++)
 {
  MPI_Recv(&offset, 1, MPI_INT, i, FROM_WORKER, MPI_COMM_WORLD, &status);
  MPI_Recv(&rows,   1, MPI_INT, i, FROM_WORKER, MPI_COMM_WORLD, &status);
  MPI_Recv(&C(offset,0), rows*NCB, MPI_DOUBLE, i, FROM_WORKER, MPI_COMM_WORLD, &status);
 }

 t2=MPI_Wtime(); // get end time's momemts

 for (i=0; i<min(NRA,NCB); i++) // calculate of matrix [C] trace
  sum_diag += C(i,i);

 printf ("\nMultiply time = %.3e sec\n\nResult matrix trace = %.3e\n", 
         t2-t1, sum_diag);

/* print results ...not needs!!!
 printf("Here is the result matrix:\n");
 for (i=0; i<NRA; i++)
 {
  printf("\n"); 
  for (j=0; j<NCB; j++) 
   printf("%.3e  ", C[i][j]);
 }
  printf ("\n");
*/

 } /* end MASTER work */

/**************************** worker task ************************************/

 if (taskid != MASTER)
 {
  MPI_Recv(&offset, 1, MPI_INT, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
  MPI_Recv(&rows,   1, MPI_INT, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
  MPI_Recv(&A(0,0), rows*NCA, MPI_DOUBLE, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
  MPI_Recv(&B(0,0), NCA*NCB,  MPI_DOUBLE, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);

  for (k=0; k<NCB; k++)
   for (i=0; i<rows; i++)
   { 
/* C[i][k] = 0.0;  by means [C] until zeroing */
    for (j=0; j<NCA; j++)
     C(i,k) += A(i,j) * B(j,k);
   }

  MPI_Send(&offset, 1, MPI_INT, MASTER, FROM_WORKER, MPI_COMM_WORLD);
  MPI_Send(&rows,   1, MPI_INT, MASTER, FROM_WORKER, MPI_COMM_WORLD);
  MPI_Send(&C(0,0), rows*NCB, MPI_DOUBLE, MASTER, FROM_WORKER, MPI_COMM_WORLD);

 } // end if(taskid != MASTER)

 MPI_Finalize(); /* end of ALL */

 free(pA); /* free memory from system */
 free(pB);
 free(pC);

} // end of program
