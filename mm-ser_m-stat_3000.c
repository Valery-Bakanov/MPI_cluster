// source code of MM-SER_M-STAT.C program

#include "mpi.h"
#include <stdio.h>
#include <sys/timeb.h> // for ftime
double f_time(void); /* define real time by ftime function  */
#include "f_time.c"

#define REAL double /* type of REAL value */

#define NRA 3000 /* number of rows in matrix A */
#define NCA 3000 /* number of columns in matrix A */
#define NCB 3000 /* number of columns in matrix B */

int main(int argc, char *argv[])
{
register
int    i,j,k;       /* indexes */
static
REAL   a[NRA][NCA], /* matrix A to be multiplied */
       b[NCA][NCB], /* matrix B to be multiplied */
       c[NRA][NCB]; /* result matrix C */
int    rc,numtasks,taskid; // for MPI system
double t1,t2; // time's momemts

/* Initialize A, B, and C matrices */
 for (i=0; i<NRA; i++)
  for (j=0; j<NCA; j++)
    a[i][j] = i+j;

 for (i=0; i<NCA; i++)
  for (j=0; j<NCB; j++)
   b[i][j] = i*j;

 for(i=0; i<NRA; i++)
  for(j=0; j<NCB; j++)
   c[i][j] = 0.0;

t1=f_time(); // get start time's moment

/* It's REALLY matrix multiply */
 for(i=0; i<NRA; i++)
  for(j=0; j<NCB; j++)
   for(k=0; k<NCA; k++)
    c[i][j] += a[i][k] * b[k][j];

t2=f_time(); // get end time's moment
printf ("Multiplay time= %.3lf sec\n\n", t2-t1);

/* ...it's a trivial program... not needs !!! 

 printf("Here is the result matrix:\n");
 for (i=0; i<NRA; i++)
 { 
  printf("\n"); 
  for (j=0; j<NCB; j++) 
   printf("%8.2f   ", c[i][j]);
 }
 printf ("\n");
*/

} // end of program 
