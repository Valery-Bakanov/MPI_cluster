#!/bin/bash

P=5 # count of processors

# COMPILATION
mpicc -o examp_01 -v examp_01.c >& examp_01.comp

# EXECUTION
mpirun -np $P examp_01 -maxtime 100

