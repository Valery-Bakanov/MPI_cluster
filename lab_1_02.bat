#!/bin/bash

P=5 # count of processors

# COMPILATION
mpicc -o mpi_test -v mpi_test.c >& mpi_test.comp

# EXECUTION
mpirun -np $P mpi_test T100 t100 R3 -maxtime 100
