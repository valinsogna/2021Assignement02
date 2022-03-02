## Compile/Debug
To compile the code, run:

`make all`

To debug and compile the cod run:

`make debug`

## Running 
This code generates a kd-tree that can be accessed by a pointer with linked lists of kd-nodes.
The code generates by default `NDATAPOINTS=10^8` data points for the kd-tree which are homogeneously random distributed among [0,`MAX`], with default `MAX=25`.

To run the code  **in serial** way with default value of `NDATAPOINTS=10^8`, simply run:

`./main.x`

Otherwise, you can specify the number of data points N as second argument:

`./main.x N`

If you want to run the code **in parallel** using N tasks, please make sure to set the proper environmental variables prior to the run command, like:

+ `export OMP_NUM_THREADS=N`
+ `export OMP_PLACES=cores`
+ `export OMP_PROC_BIND=true`
+ `export OMP_WAIT_POLICY=active`
+ `export OMP_DYNAMIC=false`
