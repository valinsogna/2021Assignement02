#!/bin/sh

#PBS -q dssc_gpu
#PBS -l nodes=1:ppn=32
#PBS -l walltime=0:30:00


cd $PBS_O_WORKDIR
rm -f kd_tree_strong_mpi_gpu.sh.*
mkdir -p results_strong_gpu

module load openmpi-4.1.1+gnu-9.3.0

OUT='/u/dssc/valinsogna/2021Assignement02/MPI/results_strong_gpu'
#'/fast/dssc/valinsogna/2021Assignement01/MPI/results_strong_gpu'

make clean
make all

for i in 1 2 4 8 16 32
#for i in {1..24};
do
    mpirun -np ${i} --map-by socket --mca btl ^openib ./main.x >> ${OUT}/stdout_${i}.txt
done
cat $PBS_NODEFILE >> ${OUT}/nodes_used.out

exit