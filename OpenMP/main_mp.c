#if defined(__STDC__)
#  if (__STDC_VERSION__ >= 199901L)
#     define _XOPEN_SOURCE 700
#  endif
#endif
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <omp.h>
#include "kd_tree_mp.h"

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
#define CPU_TIME (clock_gettime( CLOCK_REALTIME, &ts ), (double)ts.tv_sec + \
		  (double)ts.tv_nsec * 1e-9)

#define CPU_TIME_th (clock_gettime( CLOCK_THREAD_CPUTIME_ID, &myts ), (double)myts.tv_sec +	\
		     (double)myts.tv_nsec * 1e-9)

void print_kdtree(kdnode *);
void printPasser(kdnode *);
kpoint *genRandomKPoints(unsigned int);

int main(int argc, char **argv){

    unsigned int n; 
    if ( argc > 1 )
        n = atoi(*(argv + 1));
    else
        n = NDATAPOINT;

    struct timespec ts;
    int ndim = NDIM;
    double tend, tstart;

    kpoint *data = genRandomKPoints(n);
    kdnode *kdtree;

    #if defined(DEBUG)
    PRINTF("Array randomly generated:\t");
    for(int j = 0; j < n; j++) {
        PRINTF("(%.2f,%.2f)\t", data[j].coord[0], data[j].coord[1]);
    }
    PRINTF("\n");
    #endif

    tstart = CPU_TIME;
    #pragma omp parallel shared(data, ndim, kdtree, n) //Just to remember which are shared
    {
        #pragma omp single nowait // No syncro barrier at the end (!= single). Or, use master!
        {
            PRINTF("Threa crating the task is %d\n",omp_get_thread_num());

            kdtree = build_kdtree(data, ndim, -1, 0, n-1);
            
        }
    }
    tend = CPU_TIME;
    //Uncomment to print tree when DEBUG but ATT: it is not adviced when n is large!
    //print_kdtree(kdtree);


    printf("The parallel kd-tree building tooks %9.3e of wall-clock time\n", tend - tstart );
    free(data);

    return 0;
}

kpoint *genRandomKPoints(unsigned int npoints){

    srand48(time(NULL));

    kpoint *points;
    kpoint temp;

    // allocate memory
    if ( (points = (kpoint*)malloc( npoints * sizeof(kpoint) )) == NULL )
       {
         printf("I'm sorry, there is not enough memory to host %lu bytes\n",
    	     npoints * sizeof( kpoint) );
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

void print_kdtree(kdnode * tree){
    if (tree == NULL){
        PRINTF("blank\n");
        return;
    }else{
        printPasser(tree);
    }
}

void printPasser(kdnode * node){
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
