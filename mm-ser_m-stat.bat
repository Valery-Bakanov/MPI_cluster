#!/bin/bash

P_MAX=5 # count of processors in system

for N in 0500 0750 1000 1500 2000 # cycles along matrice dimensions
do

 mpicc -o mm-ser_m-stat_$N -v mm-ser_m-stat_$N.c  >& mm-ser_m-stat_$N.comp # compilation

 out_dir=`pwd`/N=$N-P=$P
 mkdir $out_dir

 mpirun -np $P_MAX -stdiodir $out_dir mm-ser_m-stat_$N -maxtime 100 # serial executions

done



