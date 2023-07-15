// source code of MM-SER_M-DYN.C program

#include "mpi.h"
#include <stdio.h>
#include <sys/timeb.h> /* for ftime */
double f_time(void); /* define real time by ftime function  */
#include "f_time.c"

int NRA = 1000, /* number of rows in matrix [A] and [C] */
    NCB = 1000, /* number of columns in matrix [B] and [C] */
    NCA = 1000; /* number of columns in matrix [A] and rows [B] */

#define REAL double /* type of REAL value */

REAL *pA, *pB, *pC; /* pointers on [A], [B] and [C] matrix */

#define A(i,k) pA[i*NCA+k] /* macros for adressing [A] matrix */
#define B(k,j) pB[k*NCB+j] /* macros for adressing [B] matrix */
#define C(i,j) pC[i*NCB+j] /* macros for adressing [C] matrix */

int main(int argc, char *argv[])
{
 register 
 int    i,j,k; /* indexes */
 int    rc,numtasks,taskid; // for MPI system
 double t1,t2; /* time's moments */

 MPI_Status status;

 rc = MPI_Init(&argc,&argv); /* get arguments of MPI-system */
 rc|= MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
 rc|= MPI_Comm_rank(MPI_COMM_WORLD,&taskid);

 if (rc != MPI_SUCCESS)
  printf ("...error initializing MPI and obtaining task ID information\n");
 else
  printf ("task ID = %d\n", taskid);

 NRA = atoi(argv[1]); /* number of rows in matrix [A] and [C] */
 NCB = atoi(argv[2]); /* number of columns in matrix [B] and [C] */
 NCA = atoi(argv[3]); /* number of columns in matrix [A] and rows [B] */

// if (taskid == MASTER)
  printf("\nOrder of multiplay matrix:\nNRA = %d\nNCB = %d\nNCA = %d\n\n", 
           NRA,NCB,NCA); 

 pA = (REAL*) calloc(NRA*NCA, sizeof(REAL)); 
 pB = (REAL*) calloc(NCA*NCB, sizeof(REAL)); 
 pC = (REAL*) calloc(NRA*NCB, sizeof(REAL));  /* allocate and zeroing memory  */

 if(!pA || !pB || !pC) /* if return value = NULL... */
 {
  printf("\n\nOut of memory for pA, pB or pC...\n\n");
  exit(-13);
 }

/* initialize [A] and [B] matrixes (matrix [Ñ] was zeroing)  */
 for (i=0; i<NRA; i++)
  for (k=0; k<NCA; k++)
   A(i,k) = i+k;

 for (k=0; k<NCA; k++)
  for (j=0; j<NCB; j++)
   B(k,j) = k*j;

 t1=f_time(); /* get start time's moment */

/* owner matrix multiply */
 for(i=0; i<NRA;i++)
  for(j=0; j<NCB;j++)
   for(k=0;k<NCA;k++)
    C(i,j) += A(i,k) * B(k,j);

 t2=f_time(); /* get end time's moment */

 printf ("Multiplay time= %.3e sec\n\n", t2-t1);

/* output calculated data ... not needs!!! 

 printf("it's [C] matrix:\n");
 for (i=0; i<NRA; i++)
 { 
  printf("\n"); 
  for (j=0; j<NCB; j++) 
   printf("%.3e ", C(i,j);
 }
   printf ("\n");
*/

 MPI_Finalize(); /* end of ALL */

 free(pA); /* deallocate memory */
 free(pB);
 free(pC);

} // end of program
