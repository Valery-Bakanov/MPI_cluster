// source code PI_SER.C program
#include <stdio.h>
#include <math.h>

#include <sys/timeb.h> // for ftime
double f_time(void); /* define real time by ftime function  */

void srandom (unsigned seed);  
long random(void);
double dboard (int darts);
#include "dboard.c" // including dboard.c file
#include "f_time.c" // including f_time.c file

#define DARTS  10000  /* number of throws at dartboard */
#define ROUNDS 100    /* number of times "darts" is iterated */

int main(int argc, char *argv[])
{
double pi,         /* average of pi after ‘darts’ is thrown */
            avepi,   /* average pi value for all iterations */
            t1, t2, // time’s moments
            pi_prec=4.0*(atanl(1.0)-atan(0.0)); // pi precision
int i, n;

t1=f_time(); // fix start time calculated

srandom (5); // init of random generator
avepi = 0;
for (i=0; i<ROUNDS; i++) // rounds times call dboard function
{
 pi = dboard(DARTS);
 avepi = ((avepi * i) + pi)/(i + 1); 
 t2=f_time(); // fix end time calculated
 printf("%7d throws aver.value of PI= %.12lf (rel.error= %.5lf %%, time= %.3lf sec)\n",  
         (DARTS * (i+1)), avepi, 1.0e2*(pi_prec-avepi)/pi_prec, t2-t1);
} // end for (i=0; i<ROUNDS; i++)  
} // end of MAIN function
// end of PI_SER.C program
