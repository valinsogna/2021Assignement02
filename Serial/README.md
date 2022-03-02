## Compile/Debug
To compile the code, run:

`make all`

To debug and compile the cod run:

`make debug`

## Running 
This code generates a kd-tree that can be accessed by a pointer with linked lists of kd-nodes.
The code generates by default `NDATAPOINTS=10^8` data points for the kd-tree which are homogeneously random distributed among [0,`MAX`], with default `MAX=25`.

To run the code  with default value of `NDATAPOINTS=10^8`, simply run:

`./main.x`

Otherwise, you can specify the number of data points N as second argument:

`./main.x N`