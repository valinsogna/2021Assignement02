## Compile/Debug
To compile the code, run:

+ `module load openmpi-4.1.1+gnu-9.3.0`
+ `make all`

To debug and compile the cod run:

+ `module load openmpi-4.1.1+gnu-9.3.0`
+ `make debug`

## Running
This code generates a kd-tree that can be accessed by a pointer with linked lists of kd-nodes.
The code generates by default `NDATAPOINTS=10^8` data points for the kd-tree which are homogeneously random distributed among [0,`MAX`], with default `MAX=25`.

To run the code  **in parallel** way with default value of `NDATAPOINTS=10^8` and N processes, simply run:

`mpirun -np N --map-by socket --mca btl ^openib ./main.x`

Otherwise, you can specify the number of data points N as second argument:

`mpirun -np N --map-by socket --mca btl ^openib ./main.x N`
