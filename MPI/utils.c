#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
// #include "kd_tree.h"
#include "utils.h"

/**
 * @brief Compute the maximum parallel depth reachable with the given set of
 *          processes/threads.
 *
 * Due to the approach used to parallelize the construction of the k-d tree,
 * there is a maximum depth in which every split is guaranteed to have at least
 * one process ready to take on the right branch. This function computes this
 * depth.
 *
 * This function determines indirectly also the number of "surplus" processes.
 *
 * @param np_size Number of processes/threads available.
 * @return int
 */
int get_max_parallel_depth(int np_size) {
  return log2((double)np_size);
}

/**
 * @brief Compute the number of surplus processes.
 *
 * A surplus process is a process assigned to a level of the tree which does
 * not guarantee that every splits has a process ready to take on the right
 * branch. Only some splits have a surplus process, starting from the leftmost
 * split in the tree.
 *
 * @param np_size Number of processes/threads available.
 * @param max_depth Maximum depth which guarantees that there is at least one
 *                    idle process.
 * @return int
 */
int get_remaining_processes(int np_size, int max_depth) {
  return np_size - (int)pow(2.0, (double)max_depth);
}

/**
 * @brief Compute the rank of the process which is going to take on the right
 *          branch after a split occurred.
 *
 * This function does not assign ranks sequentially. This is an example of the
 * expected output for 10 processes (i.e. max_depth=2).
 * --- level 0 ----
 * 0 -> 4
 * --- level 1 ----
 * 0 -> 2
 * 4 -> 6
 * --- level 2 ----
 * 0 -> 1
 * 2 -> 3
 * 4 -> 5
 * 6 -> 7
 *
 * Then surplus processes come to play:
 * 0 -> 8
 * 1 -> 9
 * 2 -> -1
 * 3 -> -1
 * ...
 *
 * @param rank Rank of the process which operated the split.
 * @param max_depth Maximum depth which guarantees that there is at least one
 *                    idle process.
 * @param next_depth Depth of the next level of the tree (the one after the
 *                    split).
 * @param surplus_processes Number of surplus processes.
 * @param np_size Number of processes/threads available.
 * @return int
 */
int get_right_process_rank(int rank, int max_depth, int next_depth, int surplus_processes, int np_size) {
  // this has two components: one for non-surplus processes, and one for surplus
  if (next_depth <= max_depth)
    return rank + pow(2.0, max_depth - next_depth);
  else if (next_depth > max_depth && rank < surplus_processes)
    return np_size - surplus_processes + rank;
  else
    return -1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Choose splitting point value for node of kd-tree
kpoint *choose_splitting_point( kpoint *points, short int axis, int N, int startIndex, int finalIndex){

    //Get median position index for odd/even arrays
    int k = 0;
    kpoint *midpoint = (kpoint*)malloc(sizeof(kpoint));
    if(N <= 30){//
        k = startIndex + N/2;
        // #if defined(DEBUG)
        //     PRINTF("PRIMA\n");
        //     for (int i=startIndex; i<=finalIndex; i++){
        //         PRINTF("points[%d]: (%.2f, %.2f)\n", i, (points + i)->coord[0], (points + i)->coord[1]);
        //     }
        // #endif
        hybrid_quick_sort(points, startIndex, finalIndex, axis);
        // #if defined(DEBUG)
        //     PRINTF("DOPO\n");
        //     for (int i=startIndex; i<=finalIndex; i++){
        //         PRINTF("points[%d]: (%.2f, %.2f)\n", i, (points + i)->coord[0], (points + i)->coord[1]);
        //     }
        // #endif
        midpoint= (points + k);
    }else{
        k = startIndex + N/2;
        double median_value = find_median(points, k, axis, startIndex, finalIndex);
        k = three_way_partition(points, median_value, axis, startIndex, finalIndex);
        midpoint->coord[axis]=median_value;
        midpoint->coord[(axis+1)%NDIM] = (points + k)->coord[(axis+1)%NDIM];
    }

    return midpoint;
}

// Dutch flag partition for recursive usage of build_kdtree
int three_way_partition(kpoint *arr, double midvalue, short int axis, int startIndex, int finalIndex) {
    int i = startIndex;
    int j = startIndex;
    int k = finalIndex;
    while (j <= k) {
        if (arr[j].coord[axis] < midvalue) {
            swap_kpoint(arr + i, arr + j);
            ++i;
            ++j;
        } else if (arr[j].coord[axis] > midvalue) {
            swap_kpoint(arr + j, arr + k);
            --k;
        } else {
            ++j;
        }
    }
    // PRINTF("j %d, k %d, i %d\n",j,k,i);
    return j-1;
}

// Smallest kth algorithm for finding the median with O(n) time complexity
double find_median(kpoint *arr, int k, short int axis, int startIndex, int finalIndex) {

    double pivot = arr[startIndex].coord[axis];
    // #if defined(DEBUG)
    // int N = finalIndex-startIndex+1;
    // PRINTF("PRIMA N %d\n", N);
    // for (int i=startIndex; i<=finalIndex; i++){
    //     PRINTF("arr[%d]: (%.2f, %.2f)\n", i, (arr + i)->coord[0], (arr + i)->coord[1]);
    // }
    // #endif

    int pos = three_way_partition(arr, pivot, axis, startIndex, finalIndex);

    // #if defined(DEBUG)
    // PRINTF("DOPO N %d\n", N);
    // for (int i=startIndex; i<=finalIndex; i++){
    //     PRINTF("arr[%d]: (%.2f, %.2f)\n", i, (arr + i)->coord[0], (arr + i)->coord[1]);
    // }
    // #endif

    if (pos == k)
        return pivot;
    if (pos > k){
        return find_median(arr, k, axis, startIndex, pos - 1);
    }
    if (pos < k){
        return find_median(arr, k, axis, pos + 1, finalIndex);
    }
    return -1;
}

// Swap func
void swap_kpoint(kpoint *a,kpoint *b){
    kpoint temp = *a;
    *a = *b;
    *b = temp;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Choose splitting dimension for node of kd-tree
short int choose_splitting_dimension( kpoint *points, int ndim, short int axis, int finalIndex, int startIndex){
    // axis is the splitting dimension from the previous call
    // ( Choose -1 for the first call)

    short int myaxis;

    //If ROUND_ROBIN_AXIS is set at true, we skip the time_complex O(n) call for function getExtent
    if(ROUND_ROBIN_AXIS){
        myaxis = (axis+1)%ndim;
        return myaxis;
    }

    float diff;

    float ext_dim1 = getExtent(points,0,finalIndex, startIndex);
    float ext_dim2 = getExtent(points,1,finalIndex, startIndex);

    diff = ext_dim2 > ext_dim1? (ext_dim2 - ext_dim1) / ext_dim2 : (ext_dim1 - ext_dim2) / ext_dim1;
    /*
    If the difference among the two dimensional extension is smaller than 50% threshold,
    keep round robin alternance of the splitting dimension
    */
    if(diff <= EXTENT_DIFF_THRESHOLD){
        myaxis = (axis+1)%ndim;
    }else{
        // otherwise keep previous dimension
        if(axis == -1){
            myaxis = 0;
            // PRINTF("Axis choice: prima %d, poi %d\n", axis, myaxis);
            return myaxis;
        }
        myaxis = axis;
    }
    // PRINTF("Axis choice: prima %d, poi %d\n", axis, myaxis);
    return myaxis;
}

// Func used in choose_splitting_dimension to avoid round-robin choice of coordinate in non homogeneus data set
double getExtent(kpoint *points, int dim,int finalIndex, int startIndex){

    double maxValue = points[startIndex].coord[dim];
    double minValue = points[startIndex].coord[dim];
    for(int j = startIndex + 1; j <=finalIndex; j++) {
        if(maxValue < points[j].coord[dim])
            maxValue = points[j].coord[dim];
        if(minValue > points[j].coord[dim])
            minValue = points[j].coord[dim];
    }
    return maxValue - minValue;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Hybrid function -> Quick + Insertion sort
void hybrid_quick_sort(kpoint *arr, int low, int high, short int axis){
    while (low < high){

    // If the size of the array is less
    // than threshold apply insertion sort
    // and stop recursion

    if (high-low + 1 < 10){
        insertion_sort(arr, low, high, axis);
        break;
    }else{
        int pivot = partition(arr, low, high, axis) ;

        // Optimised quicksort which works on
        // the smaller arrays first

        // If the left side of the pivot
        // is less than right, sort left part
        // and move to the right part of the array

        if (pivot-low<high-pivot){
            hybrid_quick_sort(arr, low, pivot - 1, axis);
            low = pivot + 1;
        }else{

        // If the right side of pivot is less
        // than left, sort right side and
        // move to the left side

        hybrid_quick_sort(arr, pivot + 1, high, axis);
        high = pivot-1;
        }

    }
    }
}

// Perfrom ascending sorting on array with few elements from Hybrid Func
void insertion_sort(kpoint *arr, int low, int n, short int axis)
  {

    for(int i=low+1;i<n+1;i++){
        kpoint val = arr[i];
        int j = i ;
        while (j>low && arr[j-1].coord[axis] > val.coord[axis]){
            arr[j]= arr[j-1] ;
            j-= 1;
        }
        arr[j] = val;
    }

  }

// Partition function for quicksort
int partition(kpoint *arr, int low, int high, short int axis)
{
  double pivot = arr[high].coord[axis];
  int j;
  j = low;

  for (int i = low; i < high; i++)
     {
      if(arr[i].coord[axis] < pivot){
        swap_kpoint(&arr[i],&arr[j]);
        j += 1;
      }
    }

    swap_kpoint(&arr[j],&arr[high]);
  return j;
  }

// Function to call the partition function and perform ascending quick sort on the array
void quick_sort(kpoint *arr, int low,int high, short int axis)
{
  if (low < high)
  {
    int pivot = partition(arr, low, high, axis);
    quick_sort(arr, low, pivot-1, axis) ;
    quick_sort(arr, pivot + 1, high, axis) ;


  }
}
