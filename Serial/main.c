#include <stdio.h>
//#include <omp.h>
#include <time.h>
#include "kd_tree.h"
#include <stdlib.h>

#define NDIM 2 //
#define MAX 25
#define NDATAPOINT 100000000 //10^8
/*
CLOCK_PROCESS_CPUTIME_ID:
amount of time a process has been running on a CPU, 
while excluding time that the process was waiting for a CPU resource on a 'run queue'.
*/
#define CPU_TIME (clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &ts ), (double)ts.tv_sec + \
		  (double)ts.tv_nsec * 1e-9)

#define CPU_REALTIME (clock_gettime( CLOCK_REALTIME, &ts ), (double)ts.tv_sec + \
		  (double)ts.tv_nsec * 1e-9)

void print_kdtree(struct kdnode *kdtree);
void printPasser(struct kdnode *node);

struct kpoint *genRandomKPoints(const int ndim, const int npoints){

    srand48(time(NULL));

    struct kpoint *points;
    struct kpoint temp;

    // allocate memory
    if ( (points = (struct kpoint*)calloc( npoints, sizeof(struct kpoint) )) == NULL )
       {
         printf("I'm sorry, there is not enough memory to host %lu bytes\n",
    	     npoints * sizeof(struct kpoint) );
         return points;
       }
    

    // initilaized random points doubles from 0 up to MAX, homogenously distributed among the 2 dimensions.
    for(int i=0; i<npoints; ++i){
        temp.coord[0] = drand48() * MAX;
        temp.coord[1] = drand48() * MAX;
        points[i] = temp;
    }

    return points;
}

int main(){

    struct timespec ts, myts;

    int ndim = NDIM, n=NDATAPOINT;

    struct kpoint *data = genRandomKPoints(ndim, n);

    // printf("Array randomly generated:\t");
    // for(int j = 0; j < n; j++) {
    //     printf("(%.2f,%.2f)\t", data[j].coord[0], data[j].coord[1]);
    // }
    // printf("\n");

    double tstart = CPU_TIME;
    struct kdnode *kdtree = build_kdtree(data, ndim, -1, 0, n-1);
    //print_kdtree(kdtree);
    double tend = CPU_TIME;

    printf("The serial kd-tree building tooks %9.3e of wall-clock time\n", tend - tstart );
    free(data);

    return 0;
}

/* Print all the elements in the linked list */
// void print_kdtree(struct kdnode *kdtree) {
//     kdtree *current_node = kdtree;
//     kdtree *left_node = current_node->left;
//     kdtree *right_node = current_node->right;
//     printf("Print tree:\n");
//     //printf("axis: %d\n", current_node->axis);
//     printf("(%.2f,%.2f)\n", current_node->split.coord[0], current_node->split.coord[1]);

//    	while (left_node != NULL && right_node != NULL) {
//         printf("\t/\t\t\t\t\t'\'\n");
//         printf("(%.2f,%.2f)\t\t\t", left_node->split.coord[0], left_node->split.coord[1]);
//         printf("(%.2f,%.2f)\n", right_node->split.coord[0], right_node->split.coord[1]);
//         right_node = right_node->right;
//         left_node = left_node->left;
//     }
// }

void print_kdtree(struct kdnode * tree){
    if (tree == NULL){
        printf("blank\n");
        return;
    }else{
        //printf("Point: (%.2f,%.2f) \n", tree->split.coord[0], tree->split.coord[1]);
        printPasser(tree);
    }
}

void printPasser(struct kdnode * node){
    if (node != NULL){
        printf("Point: (%.2f,%.2f) \n", node->split.coord[0], node->split.coord[1]);
    }else{
        return;
    }

    if (node->left != NULL){
        printPasser(node->left);
    }else{
        printf("blank\n");
    }
    if (node->right != NULL){
        printPasser(node->right);
    }else{
        printf("blank\n");
    }
}
