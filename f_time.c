double f_time() /* return time since 01.I.1970 as double in ‘sec,msec’ */
{
 struct timeb tp;
 ftime(&tp);
 return((double)(tp.time)+1.0e-3*(double)(tp.millitm));
} // end f_time function
