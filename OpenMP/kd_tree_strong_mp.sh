#!/bin/sh

#PBS -q dssc_gpu
#PBS -l nodes=1:ppn=24
#PBS -l walltime=1:10:00


cd $PBS_O_WORKDIR
rm -f kd_tree_strong_mp.sh.*
mkdir -p results_strong

export OMP_PLACES=cores
export OMP_PROC_BIND=true
export OMP_WAIT_POLICY=active
export OMP_DYNAMIC=false

make clean
make all

OUT='/u/dssc/valinsogna/2021Assignement02/OpenMP/results_strong'
#'/fast/dssc/valinsogna/2021Assignement01/OpenMP/results_strong'

for i in {1..24};
do
    export OMP_NUM_THREADS=${i}
    ./main_mp.x >> ${OUT}/stdout_${i}.txt
done
cat $PBS_NODEFILE >> ${OUT}/nodes_used.out

exit