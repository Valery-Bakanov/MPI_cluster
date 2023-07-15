#!/bin/bash

P=5 # count processors

X=1 #lenght of message (bites)

# COMPILATION
mpicc -o prog_mpi -v prog_mpi.c >& prog_mpi.comp

# EXECUTION
for pow in $(seq 0 21) # cycles by power of X

do
 
 out_dir=`pwd`/X=2^$pow=$X'_'P=$P
 mkdir $out_dir

 mpirun -np $P -stdiodir $out_dir prog_mpi $X # parallel run

 ((X=2*X)) # multiplay on 2

done
