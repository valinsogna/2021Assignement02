#if defined(__STDC__)
#  if (__STDC_VERSION__ >= 199901L)
#     define _XOPEN_SOURCE 700
#  endif
#endif
#include <stdio.h>
#include <time.h>
#include "kd_tree.h"
#include <stdlib.h>
#include "medOfMed.h"

#if defined(DEBUG)
#define PRINTF(...) printf(__VA_ARGS__);
#else
#define PRINTF(...)
#endif
#define NDIM 2
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

struct kpoint *genRandomKPoints(int npoints){

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

int main(int argc, char **argv){

    int n; 
    if ( argc > 1 )
        n = atoi(*(argv + 1));
    else
        n = NDATAPOINT;

    struct timespec ts;
    int ndim = NDIM;
    FILE *fptr;

    struct kpoint *data = genRandomKPoints(n);

    #if defined(DEBUG)
    PRINTF("Array randomly generated:\t");
    for(int j = 0; j < n; j++) {
        PRINTF("(%.2f,%.2f)\t", data[j].coord[0], data[j].coord[1]);
    }
    PRINTF("\n");
    #endif

    double tstart = CPU_TIME;
    struct kdnode *kdtree = build_kdtree(data, ndim, -1, 0, n-1);
    //Uncomment to print tree when DEBUG but ATT: it is not adviced when n is large!
    //print_kdtree(kdtree);
    double tend = CPU_TIME;
    //double time = ;

    printf("The serial kd-tree building tooks %9.3e of wall-clock time\n", tend - tstart );
    fptr = fopen("./time.dat","a");
    if(fptr == NULL){
        fprintf(stderr, RED "[ERROR]"
            NC  "Could not open file ./time.out\n"
	    );
        exit(EXIT_FAILURE);         
    }
    fprintf(fptr,"%9.3e\n",tend - tstart);
    fclose(fptr);

    free(data);
    free(kdtree);

    return 0;
}

void print_kdtree(struct kdnode * tree){
    if (tree == NULL){
        PRINTF("blank\n");
        return;
    }else{
        printPasser(tree);
    }
}

void printPasser(struct kdnode * node){
    if (node != NULL){
        PRINTF("Point: (%.2f,%.2f) \n", node->split.coord[0], node->split.coord[1]);
    }else{
        return;
    }

    if (node->left != NULL){
        printPasser(node->left);
    }else{
        PRINTF("blank\n");
    }
    if (node->right != NULL){
        printPasser(node->right);
    }else{
        PRINTF("blank\n");
    }
}
