#!/bin/sh

#PBS -q dssc
#PBS -l nodes=1:ppn=24
#PBS -l walltime=0:30:00


cd $PBS_O_WORKDIR
rm -f kd_tree_week_mpi.sh.*
mkdir -p results_week

module load openmpi-4.1.1+gnu-9.3.0

OUT='/u/dssc/valinsogna/2021Assignement02/MPI/results_week'
#'/fast/dssc/valinsogna/2021Assignement01/MPI/results_week'

make clean
make all

start=10000000 #10^7

for i in 1 2 4 8 16
#for i in {1..24};
do
    a=$(( i * start ))
    mpirun -np ${i} --map-by socket --mca btl ^openib ./main.x ${a} >> ${OUT}/stdout_${i}.txt
done
cat $PBS_NODEFILE >> ${OUT}/nodes_used.out

exit