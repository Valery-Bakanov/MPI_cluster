#!/bin/bash

# DELETE NOT NEED DIRECTORIES IN lab_xy.bat

# rm -r mm_mpi_* 


max_idiots=1000

for ext in $(seq 1 $max_idiots) # delete LAB_1_01
do
 if   [ -d examp_01.$ext ];   # if catalogue exist
 then rm -r examp_01.$ext; fi # delete catalogue  
done
if   [ -a examp_01 ]; # if file exist
then rm examp_01; fi  # delete file

for ext in $(seq 1 $max_idiots) # delete LAB_1_02
do
 if   [ -d mpi_test.$ext ];   # if catalogue exist
 then rm -r mpi_test.$ext; fi # delete catalogue  
done
if   [ -a mpi_test ]; # if file exist
then rm mpi_test; fi  # delete file

for ext in $(seq 1 $max_idiots) # delete LAB_1_03
do
 if   [ -d prog_mpi.$ext ];   # if catalogue exist
 then rm -r prog_mpi.$ext; fi # delete catalogue  
done
if   [ -a prog_mpi ]; # if file exist
then rm prog_mpi; fi  # delete file

for ext0 in 0400 0800 1600 \
            0500 1000 2000 \
	    0600 1200 2400 \
	    0700 1400 2800 \
	    0800 1600 3200 \
    	    0900 1800 3600 \
    	    1000 2000 4000 
do
 for ext1 in $(seq 1 $max_idiots)
 do
  if   [ -d mm_mpi_$ext0.$ext1 ];   # if catalogue exist
  then rm -r mm_mpi_$ext0.$ext1; fi # delete catalogue  
 done
  if   [ -a mm_mpi_$ext0 ]; # if file exist
  then rm mm_mpi_$ext0; fi  # delete file
done

rm *.comp # delete all compilation messages
