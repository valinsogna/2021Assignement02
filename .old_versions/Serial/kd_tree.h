#ifndef KD_TREE_H
#define KD_TREE_H

#define NDIM 2

struct kpoint {//
    double coord[NDIM];
};
struct kdnode {
   int axis; // the splitting dimension
   struct kpoint split; // the splitting element
   struct kdnode *left, *right; // the left and right sub-trees
};

struct kdnode * build_kdtree( struct kpoint *points, int ndim, int axis , int startIndex, int finalIndex);

#endif