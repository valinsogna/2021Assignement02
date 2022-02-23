#!/bin/sh

#PBS -q dssc_gpu
#PBS -l nodes=1:ppn=24
#PBS -l walltime=1:10:00


cd $PBS_O_WORKDIR
rm -f kd_tree_strong_mpi.sh.*
mkdir -p results_strong

module load openmpi-4.1.1+gnu-9.3.0

OUT='/u/dssc/valinsogna/2021Assignement02/MPI/results_strong'
#'/fast/dssc/valinsogna/2021Assignement01/MPI/results_strong'

make clean
make all

for i in {1..24};
do
    mpirun -np ${i} --map-by socket --mca btl ^openib ./main_mpi.x >> ${OUT}/stdout_${i}.txt
done
cat $PBS_NODEFILE >> ${OUT}/nodes_used.out

exit