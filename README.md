# 2021Assignement02

## Introduction
In the following folders you have the **C-implementations** (in serial and parallel ways) of a **kd-tree generation program in two dimensions** `NDIM=2` with default `NDATAPOINTS=10^8` **double** variables **homogeneously distributed** among [0,`MAX`], with default `MAX=25`.

The generated kd-tree that can be accessed by a pointer with a linked lists of kd-nodes: the first address being the one that corrispondes to the first splitting kd-node at depth 1 of the kd-tree. 

The implementation of the algorithm for finding the suitable splitting data point in a specific dimension, consists in the QuickSelect algorithm for finding the median.

### QuickSelect

Like QuickSort, in an array of size N we pick as a pivot the first element of the array, then move the pivot element to its correct position and partition the surrounding array with a 3-ways partition.
The idea is, not to do complete QuickSort, but stop at the point where pivot itself is k’th smallest element: in our case k=N/2 is the median if N is odd or k=N/2 +1 if N is even. 
The worst case time complexity of this method is O(n^2), but it works in O(n) on average. 

### Setting variables

If the variable `ROUND_ROBIN_AXIS=1` is set at true in file `kd_tree.c`, then the choice for the splitting dimension will be alternative between the two, otherwise it will depend on the amount of extention of the data in each dimension.

If the variable `PRINT_TREE=1` is set at true in file `main.c`, then the program will print in the standard output eventually the kd-nodes starting from depth 1 to `MAX_DEPTH=4` by default.

### Others

For further instructions look into folders:

+ Serial
+ OpenMP
+ MPI

The report is presented in folder:

+ Report


