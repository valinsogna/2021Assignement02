#if defined(__STDC__)
#  if (__STDC_VERSION__ >= 199901L)
#     define _XOPEN_SOURCE 700
#  endif
#endif
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "kd_tree.h"
#include <omp.h>

#if defined(DEBUG)
#define PRINTF(...) printf(__VA_ARGS__);
#else
#define PRINTF(...)
#endif
#define NDIM 2
#define MAX 25
#define NDATAPOINT 100000000 //10^8
#define PRINT_TREE 1
#define MAX_DEPTH 4
/*
CLOCK_PROCESS_CPUTIME_ID:
amount of time a process has been running on a CPU, 
while excluding time that the process was waiting for a CPU resource on a 'run queue'.
*/
#define CPU_TIME (clock_gettime( CLOCK_REALTIME, &ts ), (double)ts.tv_sec + \
		  (double)ts.tv_nsec * 1e-9)

#define CPU_TIME_th (clock_gettime( CLOCK_THREAD_CPUTIME_ID, &myts ), (double)myts.tv_sec +	\
		     (double)myts.tv_nsec * 1e-9)

kpoint *genRandomKPoints(unsigned int);
void printTree(kdnode*, unsigned int);

int main(int argc, char **argv){

    unsigned int n; 
    if ( argc > 1 )
        n = atoi(*(argv + 1));
    else
        n = NDATAPOINT;

    struct timespec ts;
    int ndim = NDIM;
    double tend, tstart;
    double time;
    FILE *fptr;

    kpoint *data = genRandomKPoints(n);

    kdnode *kdtree;

    //#if defined(DEBUG)
    //PRINTF("Array randomly generated:\t");
    //for(unsigned int j = 0; j < n; j++) {
        //PRINTF("(%.2f,%.2f)\t", data[j].coord[0], data[j].coord[1]);
    //}
    //PRINTF("\n");
    //#endif

    tstart = CPU_TIME;
    #pragma omp parallel shared(data, ndim, kdtree, n) //Just to remember which are shared
    {
        #pragma omp single nowait // No syncro barrier at the end (!= single). Or, use master!
        {
            PRINTF("The crating the task is %d\n",omp_get_thread_num());

            kdtree = build_kdtree(data, ndim, -1, 0, n-1);
            
        }
    }
    tend = CPU_TIME;

    time = tend - tstart;
    
    printf("The parallel kd-tree building tooks %9.3e of wall-clock time\n", time );

    if(PRINT_TREE){
        unsigned int depth = 1;
        printf("The nodes are the following:\n");
        printTree(kdtree, depth);
    }
    printf("\n");


    fptr = fopen("./time.dat","a");
    if(fptr == NULL){
        fprintf(stderr, RED "[ERROR]"
            NC  "Could not open file ./time.out\n"
	    );
        exit(EXIT_FAILURE);         
    }
    fprintf(fptr,"%9.3e\n",time);
    fclose(fptr);

    free(kdtree);
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
    for(unsigned int i=0; i<npoints; ++i){
        temp.coord[0] = drand48() * MAX;
        temp.coord[1] = drand48() * MAX;
        points[i] = temp;
    }

    return points;
}

void printTree(kdnode *data, unsigned int depth) {

    if(depth > MAX_DEPTH)
        return;

    if ((data->right == NULL) && (data->left == NULL)) {
        kpoint point = data->split;
        printf("(%.2f,%.2f) -> LEAF \n", point.coord[0], point.coord[1]);
        return;
    }

    if (data->right == NULL) {
        kpoint point = data->split;
        kdnode *left = data->left;
        kpoint l_point = left->split;
        printf("(%.2f,%.2f) -> L: (%.2f,%.2f) [axis=%d] \n", point.coord[0], point.coord[1], l_point.coord[0], l_point.coord[1], data->axis);

        ++depth;
        printTree(left, depth);
        return;
    }

    if (data->left == NULL) {
        kpoint point = data->split;
        kdnode *right = data->right;
        kpoint r_point = right->split;
        printf("(%.2f,%.2f) -> R: (%.2f,%.2f) [axis=%d]\n", point.coord[0], point.coord[1], r_point.coord[0], r_point.coord[1], data->axis);

        ++depth;
        printTree(right, depth);
        return;
    }

    kpoint point = data->split;
    kdnode *right = data->right;
    kdnode *left = data->left;

    kpoint r_point = right->split;
    kpoint l_point = left->split;

    printf("(%.2f,%.2f) -> R: (%.2f,%.2f) [axis=%d] \n", point.coord[0], point.coord[1], r_point.coord[0], r_point.coord[1], data->axis);
    printf("(%.2f,%.2f) -> L: (%.2f,%.2f) [axis=%d] \n", point.coord[0], point.coord[1], l_point.coord[0], l_point.coord[1], data->axis);

    ++depth;
    printTree(left, depth);
    printTree(right, depth);
    return;
}
