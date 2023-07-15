#!/bin/bash

# compile + run program
# 1 param. is C-file name (without .C) 
# 2 param. is count processors
# 3 param. is time or run
# compile protocol  > *.comp , 

mpicc $1.c -o $1 -v >& $1.comp

mpirun -np ${2:-4} -maxtime ${3:-1} $1
