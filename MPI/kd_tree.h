#ifndef KD_TREE_H
#define KD_TREE_H

#define NDIM 2
#define RED "\e[0;31m"
#define NC "\e[0m"
#include <mpi.h>

typedef struct kdnode kdnode;
typedef struct kpoint kpoint;

struct kpoint{
    double coord[NDIM];
};

struct kdnode{
   short int axis; // the splitting dimension
   kpoint split; // the splitting element
   kdnode *left, *right; // the left and right sub-trees
};

kdnode *serial_build_kdtree(kpoint *points, int ndim, short int axis, int startIndex, int finalIndex);
kdnode *build_kdtree( kpoint *points, int ndim, short int axis , int startIndex, int finalIndex, MPI_Comm comm, int np_size, int rank, int depth, int max_depth, int surplus_np);
kdnode *prepare_build(MPI_Comm comm);
#endif