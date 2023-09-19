# Parallel KD-Tree Construction using MPI and OpenMP

A project aimed at efficiently building kd-trees in parallel using the MPI and OpenMP paradigms.

## Introduction

A kd-tree is a space-partitioning data structure for organizing points in a k-dimensional space. This project presents both serial and parallel ways of constructing the kd-tree using MPI (Message Passing Interface) and OpenMP (Open Multi-Processing).

## Features

- Generation of datasets consisting of k-points to build the kd-tree.
- Serial and parallel kd-tree construction.
- Option to visualize and print the resulting kd-tree.
- Performance evaluation tools to measure strong and weak scalability.

## Implementation Details
In the following folders you have the **C-implementation** (in serial and parallel ways) of a **kd-tree generation program in two dimensions** `NDIM=2` with default `NDATAPOINTS=10^8` **double variables homogeneously distributed** among [0,`MAX`], with default `MAX=25`.

The generated kd-tree that can be accessed by a pointer with a **linked lists of kd-nodes**: the first address being the one that corrispondes to the first splitting kd-node at depth 1 of the kd-tree. 

The implementation of the algorithm for finding the suitable splitting data point in a specific dimension, consists in the QuickSelect algorithm for finding the median.

### OpenMP Approach

- Shared `kd_tree` pointer among threads.
- Efficient task distribution among available threads for tree building.

### MPI Approach

- Uses a process distribution pattern to assign idle processes to the right branch.
- Utilizes MPI_Send and MPI_Recv for inter-process communication.

### QuickSelect

Like QuickSort, in an array of size N we pick as a pivot the first element of the array, then move the pivot element to its correct position and partition the surrounding array with a 3-ways partition.
The idea is, not to do complete QuickSort, but stop at the point where pivot itself is k’th smallest element: in our case k=N/2 is the median if N is odd or k=N/2 +1 if N is even. 
The worst case time complexity of this method is O(n^2), but it works in O(n) on average. 

### Setting variables

If the variable `ROUND_ROBIN_AXIS=1` is set at true in file `kd_tree.c`, then the choice for the splitting dimension will be alternative between the two, otherwise it will depend on the amount of extention of the data in each dimension with the default threshold set as `EXTENT_DIFF_THRESHOLD=0.5`.

If the variable `PRINT_TREE=1` is set at true in file `main.c`, then the program will print in the standard output eventually the kd-nodes starting from depth 1 to `MAX_DEPTH=4` by default.

## Scalability Performance

### Strong Scalability

- Consists of 10 measurements for varied parameters.
- Both MPI and OpenMP exhibit good scalability, with OpenMP slightly outperforming MPI.

### Weak Scalability

- Measures the scalability as both the workload and processes/threads increase.
- Both paradigms don't scale well due to initial workload imbalances.

## Areas of Improvement

- Address the issue of false sharing in the OpenMP version.
- Extend the MPI version to support any number of processes.
- Introduce a hybrid approach combining MPI and OpenMP.
- Parallelize the QuickSelect algorithm in OpenMP.
- Further code optimization opportunities such as loop unrolling.

### Others

For further instructions look into folders:

+ Serial
+ OpenMP
+ MPI

The report is presented in folder:

+ Report


