#!/bin/bash

P_MAX=5 # max count processors in system

 mpicc -o mm-mpi_m-dyn -v mm-mpi_m-dyn.c  >& mm-mpi_m-dyn.comp # compilation

for N in 500 1000 1500 2000 # cycles along matrice dimensions
do

 for P in $(seq 2 $P_MAX) # cycles to processors count

 do

  out_dir=`pwd`/N=$N'_'P=$P
  mkdir $out_dir

  mpirun -np $P -stdiodir $out_dir mm-mpi_m-dyn $N $N $N # parallel executions

 done

done



