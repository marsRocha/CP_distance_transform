#!/bin/sh
#
#PBS -N CP-teste
#PBS -l walltime=05:00 //tempo de execução
#PBS -l nodes=1:r652:ppn=1
#PBS -q mei

module load gcc/5.3.0
make