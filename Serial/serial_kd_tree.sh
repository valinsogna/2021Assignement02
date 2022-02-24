#!/bin/bash 
 
#PBS -q dssc_gpu
#PBS -l nodes=1:ppn=24
#PBS -l walltime=0:30:00

cd $PBS_O_WORKDIR 
mkdir -p results_lin
rm -f serial_kd_tree.sh.*
make clean
make all

OUT='/u/dssc/valinsogna/2021Assignement02/Serial/results_lin'
#'/fast/dssc/valinsogna/2021Assignement02/Serial/results_lin'
#pbsnodes -ajS
#qsub -l nodes=1:ppn=12 -I -l walltime=0:10:00 -q dssc_gpu

start=10

for i in {1..24};
do
    a=$(( i * start ))
    ./main_mp.x ${a} >> ${OUT}/stdout_${i}.txt
done
# ./main.x >> ${OUT}/stdout.txt
cat $PBS_NODEFILE >> ${OUT}/nodes_used.out

exit