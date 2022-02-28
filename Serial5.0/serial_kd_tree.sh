#!/bin/bash 
 
#PBS -q dssc_gpu
#PBS -l nodes=1:ppn=24
#PBS -l walltime=0:30:00

cd $PBS_O_WORKDIR 
mkdir -p results_lin_log
rm -f serial_kd_tree.sh.*
module load gnu/9.3.0
make clean
make all

OUT='/u/dssc/valinsogna/2021Assignement02/Serial5.0/results_lin_log'
#'/fast/dssc/valinsogna/2021Assignement02/Serial5.0/results_lin_log'
#pbsnodes -ajS
#qsub -l nodes=1:ppn=12 -I -l walltime=0:30:00 -q dssc_gpu

#start=100

for i in 100 1000 10000 100000 1000000 10000000 100000000;
do
    #a=$(( i * start ))
    ./main.x ${i} >> ${OUT}/stdout_${i}.txt
done
# ./main.x >> ${OUT}/stdout.txt
cat $PBS_NODEFILE >> ${OUT}/nodes_used.out

exit