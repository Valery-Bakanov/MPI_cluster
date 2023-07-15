#define sqr(x) ((x)*(x))
long random(void);

double dboard(int darts)
{
double x_coord,    /* x coordinate, between -1 and 1  */
       y_coord,    /* y coordinate, between -1 and 1  */
       pi,         /* pi  */
       r;          /* random number between 0 and 1  */
int score,   /* number of darts that hit circle */
           n;
unsigned long cconst; 

cconst = 2 << (31 - 1); /* used to convert integer random number                                         
                               between 0 and 2^31 to double random number between 0 and 1 */
score = 0; // start summ

for (n=1; n<=darts; n++)  // cicles at random 2D-points
{
 r = (double)random() / cconst;
 x_coord = (2.0 * r) - 1.0;
 r = (double)random() / cconst;
 y_coord = (2.0 * r) - 1.0;
 if ((sqr(x_coord) + sqr(y_coord)) <= 1.0) // 2D-point [x_coord, y_coord] in circle?
   score++;
} // end for (n=1; n<=darts; n++)
 pi = 4.0 * (double)score / (double)darts;
 return(pi);
} // end of dboard function
