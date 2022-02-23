#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "kd_tree.h"
#include "medOfMed.h"

#define EXTENT_DIFF_THRESHOLD 0.1
#if defined(DEBUG)
#define PRINTF(...) printf(__VA_ARGS__);
#else
#define PRINTF(...)
#endif

int choose_splitting_dimension( struct kpoint *, int, int, int);
double getExtent(struct kpoint *, int, int);
struct kpoint *choose_splitting_point( struct kpoint *, int, int, int, int);

struct kdnode *build_kdtree( struct kpoint *points, int ndim, int axis, int startIndex, int finalIndex ){
    /*
    * points is a pointer to the relevant section of the data set;
    * N is the number of points to be considered, from points to points+N * ndim is the number of dimensions of the data points
    * axis is the dimension used previsously as the splitting one
    */

    int N = finalIndex-startIndex+1; // Number of elements in arr[startIndex..startIndex]
    
    if( N >= 0){
           // Allocate the memory for a new node with classical linked-list:
        struct kdnode *node;
        if ( (node = (struct kdnode*)malloc(sizeof(struct kdnode))) == NULL )
        {
            fprintf(stderr, RED "[ERROR]"
                NC  "I'm sorry, there is not enough memory to host %lu bytes\n",
	         sizeof(struct kdnode) );
            exit(EXIT_FAILURE);
        }

        if( N == 1 ) {
            PRINTF("Array has size %d and is composed by:\n", N);
            PRINTF("(%.2f,%.2f)\t", points[startIndex].coord[0], points[startIndex].coord[1]);
            PRINTF("\n");
            // return a leaf with the point *points;
            node->left = NULL;
            node->right = NULL;
            node->axis = axis; //not interesting: I keep latest axis value.
            node->split.coord[0] = points[startIndex].coord[0];
            node->split.coord[1] = points[startIndex].coord[1];
            //printf("N=1: (%.2f, %.2f)\n",points[0].coord[0],points[0].coord[1]);
        }else{
            // implement the choice for splitting point and dimension
            int myaxis = choose_splitting_dimension( points, ndim, axis, N); //the splitting dimension
            struct kpoint *mypoint = choose_splitting_point( points, myaxis, N, finalIndex, startIndex); //the splitting point

            // We individuate the left- and right- points with a 2-ways partition.
            // OSS. points vector is already partitioned among the direction myaxis thanks to choose_splitting_point func:
            int j=startIndex;
            int notFound=1;
            while (j<=finalIndex && notFound){
                points[j].coord[myaxis] == mypoint->coord[myaxis] ? notFound = 0 : j++;
            }
            //printf("j found at: %d \n", j);
            int N_left = j - startIndex;
            int N_right= finalIndex - j;

            // printf("Array has size %d and is composed by:\n", N);
            // for(int j = startIndex; j < finalIndex +1 ; j++) {
            //     printf("(%.2f,%.2f)\t", points[j].coord[0], points[j].coord[1]);
            // }
            // printf("\n");

            node->axis = myaxis;
            node->split = *mypoint; // here we save a data point

            if(N_left == 0){
                node->left = NULL;
                //printf("L NULL: j %d | N %d | N_left %d| N_right %d\n",j, N, N_left, N_right);
            }else if(N_left > 0){
                // printf("L: j %d | N %d | N_left %d| N_right %d\n",j, N, N_left, N_right);
                // printf("L: Start %d | End %d\n",startIndex, j - 1);
                node->left = build_kdtree( points, ndim, myaxis, startIndex, j - 1 );
            }

            if(N_right == 0){
                node->right = NULL;
                //printf("R NULL: j %d | N %d | N_left %d| N_right %d\n",j, N, N_left, N_right);
            }else if(N_right > 0){
                // printf("R: j %d | N %d | N_left %d| N_right %d\n",j, N, N_left, N_right);
                // printf("R: Start %d | End %d\n",j + 1, j + N_right);
                node->right = build_kdtree( points, ndim, myaxis, j + 1, j + N_right);
            }
        }
        //printf("N>=0: (%.2f, %.2f) axis %d\n", node->split.coord[0], node->split.coord[1], node->axis);
        return node;
    }

    // If k is more than the number of elements in the array
    fprintf(stderr, RED "[ERROR]"
            NC  ": N turned out to be negative! \n");
    exit(EXIT_FAILURE);


}

struct kpoint *choose_splitting_point( struct kpoint *points, int axis, int N, int finalIndex, int startIndex){

    //Get median position index for odd/even arrays
    int k = N/2;

    struct kpoint *midpoint = getMedian(points, startIndex, finalIndex, k, axis);

    return midpoint;
}

int choose_splitting_dimension( struct kpoint *points, int ndim, int axis, int N){
    // axis is the splitting dimension from the previous call
    // ( Choose -1 for the first call)

    int myaxis;
    float diff;

    double ext_dim1 = getExtent(points,0,N);
    double ext_dim2 = getExtent(points,1,N);

    diff = ext_dim2 > ext_dim1? (ext_dim2 - ext_dim1) / ext_dim2 : (ext_dim1 - ext_dim2) / ext_dim1;
    /*
    If the difference among the two dimensional extension is smaller than 20% threshold,
    keep round robin alternance of the splitting dimension
    */
    if(diff <= EXTENT_DIFF_THRESHOLD){
        myaxis = (axis+1)%ndim;
    }else{
        // otherwise keep previous dimension
        if(axis == -1){
            //printf("Axis choice: prima %d, poi %d\n", axis, myaxis);
            return 0;
        }
        myaxis = axis;
        //PRINTF("Warning: threashold for dimension extent missed\n");
    }
    //printf("Axis choice: prima %d, poi %d\n", axis, myaxis);
    return myaxis;
}

double getExtent(struct kpoint *points, int dim, int N){
    
    double maxValue = points[0].coord[dim];
    for(int j = 1; j < N; j++) {
        if(maxValue < points[j].coord[dim])
            maxValue = points[j].coord[dim];
    }
    return maxValue;
}
