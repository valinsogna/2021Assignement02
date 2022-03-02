## Compile/Debug
To compile the code, run:

`make all`

To debug and compile the cod run:

`make debug`

## Running 
To run the code with $10^8$ homogeneously random generated data points around [0,25] **in serial** way, simply run:

`./main.x`

Otherwise, you can specify the number of data points N as second argument:

`./main.x N`

If you want to run the code **in parallel** using N tasks, please make sure to set the proper environmental variables prior to the run command, like:

+ `export OMP_NUM_THREADS=N`
+ `export OMP_PLACES=cores`
+ `export OMP_PROC_BIND=true`
+ `export OMP_WAIT_POLICY=active`
+ `export OMP_DYNAMIC=false`
