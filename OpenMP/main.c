#if defined(__STDC__)
#  if (__STDC_VERSION__ >= 199901L)
#     define _XOPEN_SOURCE 700
#  endif
#endif
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "kd_tree.h"

#if defined(DEBUG)
#define PRINTF(...) printf(__VA_ARGS__);
#else
#define PRINTF(...)
#endif
#define NDIM 2
#define MAX 25
#define NDATAPOINT 10 //100000000 //10^8
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
    FILE *fptr;

    kpoint *data = genRandomKPoints(n);
    /*
    kpoint data[33];

    data[0].coord[0] = 22.93; data[0].coord[1] = 14.36; 
    data[1].coord[0] = 24.84; data[1].coord[1] = 5.36; 
    data[2].coord[0] = 6.37; data[2].coord[1] = 4.97; 
    data[3].coord[0] = 22.96; data[3].coord[1] = 17.44; 
    data[4].coord[0] = 13.91; data[4].coord[1] = 6.96; 
    data[5].coord[0] = 20.44; data[5].coord[1] = 6.80; 
    data[6].coord[0] = 13.01; data[6].coord[1] = 24.11; 
    data[7].coord[0] = 0.73; data[7].coord[1] = 16.82; 
    data[8].coord[0] = 12.13; data[8].coord[1] = 14.43; 
    data[9].coord[0] = 17.42; data[9].coord[1] = 16.86; 
    data[10].coord[0] = 16.60; data[10].coord[1] = 9.39; 
    data[11].coord[0] = 21.85; data[11].coord[1] = 3.30; 
    data[12].coord[0] = 24.42; data[12].coord[1] = 0.54; 
    data[13].coord[0] = 22.39; data[13].coord[1] = 5.24; 
    data[14].coord[0] = 4.05; data[14].coord[1] = 1.44; 
    data[15].coord[0] = 21.01; data[15].coord[1] = 18.90; 
    data[16].coord[0] = 9.13; data[16].coord[1] = 24.44; 
    data[17].coord[0] = 4.55; data[17].coord[1] = 2.00; 
    data[18].coord[0] = 16.82; data[18].coord[1] = 4.56; 
    data[19].coord[0] = 17.02; data[19].coord[1] = 19.04; 
    data[20].coord[0] = 12.57; data[20].coord[1] = 1.14; 
    data[21].coord[0] = 8.09; data[21].coord[1] = 8.10; 
    data[22].coord[0] = 14.58; data[22].coord[1] = 2.16;
    data[23].coord[0] = 16.54; data[23].coord[1] = 11.53; 
    data[24].coord[0] = 10.66; data[24].coord[1] = 3.59; 
    data[25].coord[0] = 6.24; data[25].coord[1] = 13.30; 
    data[26].coord[0] = 4.21; data[26].coord[1] = 3.18; 
    data[27].coord[0] = 9.97; data[27].coord[1] = 9.94; 
    data[28].coord[0] = 0.31; data[28].coord[1] = 5.09; 
    data[29].coord[0] = 2.53; data[29].coord[1] = 14.64; 
    data[30].coord[0] = 20.62; data[30].coord[1] = 20.32; 
    data[31].coord[0] = 12.36; data[31].coord[1] = 18.79;
    data[32].coord[0] = 24.22; data[32].coord[1] = 2.73; 
    */
    kdnode *kdtree;

    //#if defined(DEBUG)
    //PRINTF("Array randomly generated:\t");
    //for(unsigned int j = 0; j < n; j++) {
        //PRINTF("(%.2f,%.2f)\t", data[j].coord[0], data[j].coord[1]);
    //}
    //PRINTF("\n");
    //#endif

    tstart = CPU_TIME;
    kdtree = build_kdtree(data, ndim, -1, 0, n-1);
    tend = CPU_TIME;

    double time = tend - tstart;
    
    if(PRINT_TREE){
        unsigned int depth = 1;
        printTree(kdtree, depth);
    }
        
    printf("The parallel kd-tree building tooks %9.3e of wall-clock time\n", time );


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
