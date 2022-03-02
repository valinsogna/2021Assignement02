#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "kd_tree.h"
#include <omp.h>

#define EXTENT_DIFF_THRESHOLD 0.5
#if defined(DEBUG)
#define PRINTF(...) printf(__VA_ARGS__);
#else
#define PRINTF(...)
#endif
#define ROUND_ROBIN_AXIS 1

short int choose_splitting_dimension(kpoint*, int, short int, int, int);
double getExtent(kpoint*, int, int, int);
kpoint *choose_splitting_point(kpoint *, short int, int, int, int);
void swap_kpoint(kpoint*, kpoint*);
double find_median(kpoint*, int, short int, int, int);
int three_way_partition(kpoint*, double , short int, int, int );

// Core function for building the kdtree
kdnode *build_kdtree(kpoint *points, int ndim, short int axis, int startIndex, int finalIndex ){
    /*
    * points is a pointer to the relevant section of the data set;
    * N is the number of points to be considered, from points to points+N * ndim is the number of dimensions of the data points
    * axis is the dimension used previsously as the splitting one
    */

    int N = finalIndex-startIndex+1; // Number of elements in arr[startIndex..startIndex]
    
    if( N >= 0){
        // Allocate the memory for a new node with a classical linked-list:
        kdnode *new_node;
        if ( (new_node = (kdnode*)malloc(sizeof(kdnode))) == NULL )
        {
            fprintf(stderr, RED "[ERROR]"
                NC  "I'm sorry, there is not enough memory to host %lu bytes\n",
	         sizeof(kdnode) );
            exit(EXIT_FAILURE);
        }

        short int myaxis = choose_splitting_dimension( points, ndim, axis, finalIndex, startIndex); //the splitting dimension

        if( N == 1 ) {// return a leaf with the point *points;
            PRINTF("Array has size %d and is composed by:\n", N);
            PRINTF("(%.2f,%.2f)\n", points[startIndex].coord[0], points[startIndex].coord[1]);

            new_node->left = NULL;
            new_node->right = NULL;
            new_node->axis = myaxis;
            new_node->split.coord[0] = points[startIndex].coord[0];
            new_node->split.coord[1] = points[startIndex].coord[1];
            PRINTF("N=1: (%.2f, %.2f) axis %d\n", new_node->split.coord[0], new_node->split.coord[1], new_node->axis);
            return new_node;
        }

        if( N == 2 ) {// return a node prior to a leaf with the point *points;
            PRINTF("Array has size %d and is composed by:\n", N);
            PRINTF("(%.2f,%.2f), (%.2f,%.2f)\n", points[startIndex].coord[0], points[startIndex].coord[1], 
            points[finalIndex].coord[0], points[finalIndex].coord[1]);

            if(points[startIndex].coord[myaxis] > points[finalIndex].coord[myaxis])
                swap_kpoint(&points[startIndex], &points[finalIndex]);
            
            
            // return a node with a leaf on the right;
            new_node->axis = myaxis;
            new_node->split.coord[0] = points[startIndex].coord[0];
            new_node->split.coord[1] = points[startIndex].coord[1];
            new_node->left = NULL;
            new_node->right = build_kdtree( points, ndim, myaxis, finalIndex, finalIndex);

            PRINTF("N=2: (%.2f, %.2f) axis %d\n", new_node->split.coord[0], new_node->split.coord[1], new_node->axis);
            return new_node;
        }
        
        // implement the choice for splitting point
        kpoint *mypoint = choose_splitting_point( points, myaxis, N, startIndex, finalIndex); //the splitting point
        

        // We individuate the left- and right- points with a 2-ways partition.
        // OSS. points vector is already partitioned among the direction myaxis thanks to choose_splitting_point func:
        int j= startIndex + N/2;
        // int notFound=1;
        // while (j<=finalIndex && notFound){
        //     points[j].coord[myaxis] == mypoint->coord[myaxis] ? notFound = 0 : j++;
        // }
        // PRINTF("j found at: %d \n", j);

        int N_left = j - startIndex;
        int N_right= finalIndex - j;

        #if defined(DEBUG)
        PRINTF("Array has size %d and is composed by:\n", N);
        for(int j = startIndex; j < finalIndex +1 ; j++) {
            PRINTF("(%.2f,%.2f)\t", points[j].coord[0], points[j].coord[1]);
        }
        PRINTF("\n");
        #endif

        new_node->axis = myaxis; //save the splitting dimension ion the new node
        new_node->split = *mypoint; // here we save the kpoint of the splitting point on the new node

        if(N_left > 0){
            PRINTF("L: j %d | N %d | N_left %d| N_right %d\n",j, N, N_left, N_right);
            PRINTF("L: Start %d | End %d\n",startIndex, j - 1);

            #pragma omp task shared(ndim, points) //No need for firstprivate(myaxis, startIndex, finalIndex): they are firstprivate!
            {

                PRINTF("Task runned by thread %d\n",omp_get_thread_num()); // Print the thread executing (not creating) the task

                new_node->left = build_kdtree( points, ndim, myaxis, startIndex, j - 1 );
            }
        }

        if(N_right > 0){
            PRINTF("R: j %d | N %d | N_left %d| N_right %d\n",j, N, N_left, N_right);
            PRINTF("R: Start %d | End %d\n",j + 1, j + N_right);

            #pragma omp task shared(ndim, points) // Same as above
            {

                PRINTF("Task runned by thread %d\n",omp_get_thread_num());

                new_node->right = build_kdtree( points, ndim, myaxis, j + 1, j + N_right);
            }
        }
        if(new_node->right != NULL || new_node->left != NULL){
            PRINTF("N>=0: (%.2f, %.2f) axis %d, R:(%.2f, %.2f), L:(%.2f, %.2f)\n", new_node->split.coord[0], new_node->split.coord[1],
            new_node->axis, new_node->right->split.coord[0], new_node->right->split.coord[1],
            new_node->left->split.coord[0], new_node->left->split.coord[1]);
        }else{
            PRINTF("N>=0: (%.2f, %.2f) axis %d\n", new_node->split.coord[0], new_node->split.coord[1],new_node->axis);
        }

        return new_node;
    }

    // If k is more than the number of elements in the array
    fprintf(stderr, RED "[ERROR]"
            NC  ": N turned out to be negative! \n");
    exit(EXIT_FAILURE);


}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Choose splitting point value for node of kd-tree
kpoint *choose_splitting_point( kpoint *points, short int axis, int N, int startIndex, int finalIndex){

    //Get median position index for odd/even arrays
    int k = 0;
    kpoint *midpoint = (kpoint*)malloc(sizeof(kpoint));
    if(N <= 30){//
        k = startIndex + N/2;
        #if defined(DEBUG)
            PRINTF("PRIMA\n");
            for (int i=startIndex; i<=finalIndex; i++){
                PRINTF("points[%d]: (%.2f, %.2f)\n", i, (points + i)->coord[0], (points + i)->coord[1]);
            }
        #endif
        hybrid_quick_sort(points, startIndex, finalIndex, axis);
        #if defined(DEBUG)
            PRINTF("DOPO\n");
            for (int i=startIndex; i<=finalIndex; i++){
                PRINTF("points[%d]: (%.2f, %.2f)\n", i, (points + i)->coord[0], (points + i)->coord[1]);
            }
        #endif
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
    PRINTF("j %d, k %d, i %d\n",j,k,i);
    return j-1;
}

// Smallest kth algorithm for finding the median with O(n) time complexity
double find_median(kpoint *arr, int k, short int axis, int startIndex, int finalIndex) {

    double pivot = arr[startIndex].coord[axis];
    #if defined(DEBUG)
    int N = finalIndex-startIndex+1;

    
    PRINTF("PRIMA N %d\n", N);
    for (int i=startIndex; i<=finalIndex; i++){
        PRINTF("arr[%d]: (%.2f, %.2f)\n", i, (arr + i)->coord[0], (arr + i)->coord[1]);
    }
    #endif

    int pos = three_way_partition(arr, pivot, axis, startIndex, finalIndex);

    #if defined(DEBUG)
    PRINTF("DOPO N %d\n", N);
    for (int i=startIndex; i<=finalIndex; i++){
        PRINTF("arr[%d]: (%.2f, %.2f)\n", i, (arr + i)->coord[0], (arr + i)->coord[1]);
    }
    #endif

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
            PRINTF("Axis choice: prima %d, poi %d\n", axis, myaxis);
            return myaxis;
        }
        myaxis = axis;
    }
    PRINTF("Axis choice: prima %d, poi %d\n", axis, myaxis);
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
 
// Function to call the partition function
// and perform ascending quick sort on the array
void quick_sort(kpoint *arr, int low,int high, short int axis)
{
  if (low < high)
  {
    int pivot = partition(arr, low, high, axis);
    quick_sort(arr, low, pivot-1, axis) ;
    quick_sort(arr, pivot + 1, high, axis) ;
   
   
  }
}

