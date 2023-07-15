#!/bin/bash

P_MAX=5 # count of processors in system

 mpicc -o mm-ser_m-dyn -v mm-ser_m-dyn.c  >& mm-ser_m-dyn.comp # compilation

for N in 500 750 1000 1500 2000 # cycles along matrice dimensions
do

 out_dir=`pwd`/N=$N-P=$P
 mkdir $out_dir

 mpirun -np $P_MAX -stdiodir $out_dir mm-ser_m-dyn $N $N $N -maxtime 100 # serial executions

done



