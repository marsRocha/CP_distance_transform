#!/bin/sh

#PBS -N "Cpteste"
#PBS -l walltime=10:00
#PBS -l nodes=1:r652:ppn=1
#PBS -q mei

module load gcc/5.3.0
cd ~/CP/
make
for i in {1..50}
do
  	echo "-"$i":" >> "./results${img}.txt"
        ./work "./Images/${img}.pgm" >> "./results${img}.txt"
done