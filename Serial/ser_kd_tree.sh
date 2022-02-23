#!/bin/bash 
 
#PBS -q dssc_gpu
#PBS -l nodes=1:ppn=24
#PBS -l walltime=0:10:00

cd $PBS_O_WORKDIR 
mkdir -p results1
rm -f ser_kd_tree.sh.*
make clean
make all

OUT='/u/dssc/valinsogna/2021Assignement02/Serial/results1'
#'/fast/dssc/valinsogna/2021Assignement02/Serial/results1'
#pbsnodes -ajS
#qsub -l nodes=1:ppn=1 -I -l walltime=0:10:00 -q dssc

./main.x >> ${OUT}/stdout.txt
cat $PBS_NODEFILE >> ${OUT}/nodes_used.out

exit