#!/bin/sh

#PBS -q dssc_gpu
#PBS -l nodes=1:ppn=48
#PBS -l walltime=0:15:00


cd $PBS_O_WORKDIR
rm -f kd_tree_strong_mp_gpu.sh.*
mkdir -p results_strong_gpu
module load gnu/9.3.0

export OMP_PLACES=cores
export OMP_PROC_BIND=true
export OMP_WAIT_POLICY=active
export OMP_DYNAMIC=false

make clean
make all

OUT='/u/dssc/valinsogna/2021Assignement02/OpenMP/results_strong_gpu'
#'/fast/dssc/valinsogna/2021Assignement01/OpenMP/results_strong_gpu'

for i in 1 2 4 8 16 24
#for i in {1..24}
do
    export OMP_NUM_THREADS=${i}
    ./main.x >> ${OUT}/stdout_${i}.txt
done
cat $PBS_NODEFILE >> ${OUT}/nodes_used.out

exit