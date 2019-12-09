#!/bin/sh

#PBS -N "Cpteste"
#PBS -l walltime=2:00:00
#PBS -l nodes=1:r781:ppn=32
#PBS -q mei

module load gcc/5.3.0
cd ~/CP/
make
for img in {256,512,1024,2048,4096}
do
  	for i in {1..10}
        do
          	./work "Images/img${img}.pgm" "${th}" >> "./resultsDynamic1T${$img}.txt
        done
done
