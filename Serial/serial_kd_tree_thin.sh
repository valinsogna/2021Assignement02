#!/bin/bash 
 
#PBS -q dssc
#PBS -l nodes=1:ppn=12
#PBS -l walltime=0:20:00

cd $PBS_O_WORKDIR 
mkdir -p thin_results_lin_log
rm -f serial_kd_tree_thin.sh.*
module load gnu/9.3.0
make clean
make all

OUT='/u/dssc/valinsogna/2021Assignement02/Serial/thin_results_lin_log'
#'/fast/dssc/valinsogna/2021Assignement02/Serial/thin_results_lin_log'
#pbsnodes -ajS
#qsub -l nodes=1:ppn=12 -I -l walltime=0:30:00 -q dssc

#start=100
for j in {1..9}
do
for i in 100 1000 10000 100000 1000000 10000000 100000000;
do
    #a=$(( i * start ))
    ./main.x ${i} >> ${OUT}/stdout_${i}.txt
done
done
# ./main.x >> ${OUT}/stdout.txt
cat $PBS_NODEFILE >> ${OUT}/nodes_used_thin.out

exit