#!/bin/bash 
 
#PBS -q dssc_gpu
#PBS -l nodes=1:ppn=48
#PBS -l walltime=0:10:00
 
cd /u/dssc/valinsogna/mpi-benchmarks/src_c
make clean 
module load intel 
make
EXE="/u/dssc/valinsogna/mpi-benchmarks/src_c/IMB-MPI1"

cd $PBS_O_WORKDIR 
mkdir -p intel_lib
cd intel_lib
mkdir -p gpu
cd gpu
mkdir -p out
cd out
OUT='/fast/dssc/valinsogna/2021Assignement01/section2/intel_lib/gpu/out'

for i in {1..10}; do 
    mpirun -np 2 -ppn=1 -env I_MPI_DEBUG 5 ${EXE} PingPong -msglog 28 | grep -v ^\# | grep -v '^$' | sed -r 's/^\s+//;s/\s+/,/g' >> ${OUT}/def.out
    #Run on 2 nodes:
    mpirun -np 2 -f $PBS_NODEFILE -env I_MPI_DEBUG 5 ${EXE} PingPong -msglog 28 | grep -v ^\# | grep -v '^$' | sed -r 's/^\s+//;s/\s+/,/g' >> ${OUT}/node.out 
    #Run on close sockets:
    mpirun -np 2 -ppn=2 -env I_MPI_DEBUG 5 -genv I_MPI_PIN_PROCESSOR_LIST 0,1 ${EXE} PingPong -msglog 28 | grep -v ^\# | grep -v '^$' | sed -r 's/^\s+//;s/\s+/,/g' >> ${OUT}/socket.out 
    #Run on same socket:
    mpirun -np 2 -ppn=2 -env I_MPI_DEBUG 5 -genv I_MPI_PIN_PROCESSOR_LIST 0,2 ${EXE} PingPong -msglog 28 | grep -v ^\# | grep -v '^$' | sed -r 's/^\s+//;s/\s+/,/g' >> ${OUT}/core.out 
 
done 
cd ..
cat $PBS_NODEFILE >> nodes_used.out

exit