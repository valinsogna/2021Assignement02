#ifndef KD_TREE_H
#define KD_TREE_H

#define NDIM 2
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

kdnode *build_kdtree( kpoint *points, int ndim, short int axis , int startIndex, int finalIndex);
void hybrid_quick_sort(kpoint *arr, int low, int high, short int axis);
void quick_sort(kpoint *arr, int low,int high, short int axis);
int partition(kpoint *arr, int low, int high, short int axis);
void insertion_sort(kpoint *arr, int low, int n, short int axis);

#endif