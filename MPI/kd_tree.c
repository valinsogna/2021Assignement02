#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "kd_tree.h"
#include "utils.h"
#include <mpi.h>

#if defined(DEBUG)
#define PRINTF(...) printf(__VA_ARGS__);
#else
#define PRINTF(...)
#endif

// Core function for building the kdtree on one process
kdnode *serial_build_kdtree(kpoint *points, int ndim, short int axis, int startIndex, int finalIndex){
    int N = finalIndex-startIndex+1; // Number of elements in points[startIndex..finalIndex]
    
    if( N >= 0){
        // Allocate the memory for a new node with a classical linked-list:
        kdnode *new_node;
        if ( (new_node = (kdnode*)malloc(sizeof(kdnode))) == NULL )
        {
            fprintf(stderr, RED "[ERROR]"
                NC  "I'm sorry, there is not enough memory to host %lu bytes\n",
	         sizeof(kdnode) );
            MPI_Finalize() ;
            exit(EXIT_FAILURE);
        }

        short int myaxis = choose_splitting_dimension( points, ndim, axis, finalIndex, startIndex); //the splitting dimension

        if( N == 1 ) {// return a leaf with the point *points;
            // PRINTF("Array has size %d and is composed by:\n", N);
            // PRINTF("(%.2f,%.2f)\n", points[startIndex].coord[0], points[startIndex].coord[1]);

            new_node->left = NULL;
            new_node->right = NULL;
            new_node->axis = myaxis;
            new_node->split.coord[0] = points[startIndex].coord[0];
            new_node->split.coord[1] = points[startIndex].coord[1];
            // PRINTF("N=1: (%.2f, %.2f) axis %d\n", new_node->split.coord[0], new_node->split.coord[1], new_node->axis);
            return new_node;
        }

        if( N == 2 ) {// return a node prior to a leaf with the point *points;
            // PRINTF("Array has size %d and is composed by:\n", N);
            // PRINTF("(%.2f,%.2f), (%.2f,%.2f)\n", points[startIndex].coord[0], points[startIndex].coord[1], 
            // points[finalIndex].coord[0], points[finalIndex].coord[1]);

            if(points[startIndex].coord[myaxis] > points[finalIndex].coord[myaxis])
                swap_kpoint(&points[startIndex], &points[finalIndex]);
            
            
            // return a node with a leaf on the right;
            new_node->axis = myaxis;
            new_node->split.coord[0] = points[startIndex].coord[0];
            new_node->split.coord[1] = points[startIndex].coord[1];
            new_node->left = NULL;
            new_node->right = serial_build_kdtree( points, ndim, myaxis, finalIndex, finalIndex);

            // PRINTF("N=2: (%.2f, %.2f) axis %d\n", new_node->split.coord[0], new_node->split.coord[1], new_node->axis);
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

        // #if defined(DEBUG)
        // PRINTF("Array has size %d and is composed by:\n", N);
        // for(int j = startIndex; j < finalIndex +1 ; j++) {
        //     PRINTF("(%.2f,%.2f)\t", points[j].coord[0], points[j].coord[1]);
        // }
        // PRINTF("\n");
        // #endif

        new_node->axis = myaxis; //save the splitting dimension ion the new node
        new_node->split = *mypoint; // here we save the kpoint of the splitting point on the new node

        if(N_left > 0){
            // PRINTF("L: j %d | N %d | N_left %d| N_right %d\n",j, N, N_left, N_right);
            // PRINTF("L: Start %d | End %d\n",startIndex, j - 1);

            new_node->left = serial_build_kdtree( points, ndim, myaxis, startIndex, j - 1 );
        }

        if(N_right > 0){
            // PRINTF("R: j %d | N %d | N_left %d| N_right %d\n",j, N, N_left, N_right);
            // PRINTF("R: Start %d | End %d\n",j + 1, j + N_right);

            new_node->right = serial_build_kdtree( points, ndim, myaxis, j + 1, j + N_right);
        }
        // if(new_node->right != NULL || new_node->left != NULL){
        //     PRINTF("N>=0: (%.2f, %.2f) axis %d, R:(%.2f, %.2f), L:(%.2f, %.2f)\n", new_node->split.coord[0], new_node->split.coord[1],
        //     new_node->axis, new_node->right->split.coord[0], new_node->right->split.coord[1],
        //     new_node->left->split.coord[0], new_node->left->split.coord[1]);
        // }else{
        //     PRINTF("N>=0: (%.2f, %.2f) axis %d\n", new_node->split.coord[0], new_node->split.coord[1],new_node->axis);
        // }

        return new_node;
    }

    // If k is more than the number of elements in the array
    fprintf(stderr, RED "[ERROR]"
            NC  ": N turned out to be negative! \n");
    MPI_Finalize() ;
    exit(EXIT_FAILURE);

}

// Core function for building the kdtree in parallel distribution workload among processes
kdnode *build_kdtree(kpoint *points, int ndim, short int axis, int startIndex, int finalIndex, MPI_Comm comm, int np_size, int rank, int depth, int max_depth, int surplus_np ){
    /*
    * points is a pointer to the relevant section of the data set;
    * N is the number of points to be considered, from points to points+N * ndim is the number of dimensions of the data points
    * axis is the dimension used previsously as the splitting one
    * startIndex and finalIndex are the indeces extremities of the ptr points involved in the current building of the kdnode.
    * comm is the communicator among the processes
    * np_size is the number of processes involved
    * rank is the process id involved in building the current kd-node
    * depth is the kd-tree depth level starting from 0.
    */

    int N = finalIndex-startIndex+1; // Number of elements in points[startIndex..finalIndex]
    int next_depth = depth + 1;
    int right_rank = get_right_process_rank(rank, max_depth, next_depth, surplus_np, np_size);
    
    if( N >= 0){
        // Allocate the memory for a new node with a classical linked-list:
        kdnode *new_node;
        if ( (new_node = (kdnode*)malloc(sizeof(kdnode))) == NULL )
        {
            fprintf(stderr, RED "[ERROR]"
                NC  "I'm sorry, there is not enough memory to host %lu bytes\n",
	         sizeof(kdnode) );
            MPI_Finalize() ;
            exit(EXIT_FAILURE);
        }

        short int myaxis = choose_splitting_dimension( points, ndim, axis, finalIndex, startIndex); //the splitting dimension

        if( N == 1 ) {// return a leaf with the point *points;

            new_node->left = NULL;
            new_node->right = NULL;
            new_node->axis = myaxis;
            new_node->split.coord[0] = points[startIndex].coord[0];
            new_node->split.coord[1] = points[startIndex].coord[1];

            return new_node;
        }

        if( N == 2 ) {// return a node prior to a leaf with the point *points;

            if(points[startIndex].coord[myaxis] > points[finalIndex].coord[myaxis])
                swap_kpoint(&points[startIndex], &points[finalIndex]);
            
            
            // return a node with a leaf on the right;
            new_node->axis = myaxis;
            new_node->split.coord[0] = points[startIndex].coord[0];
            new_node->split.coord[1] = points[startIndex].coord[1];
            new_node->left = NULL;
            new_node->right = build_kdtree( points, ndim, myaxis, finalIndex, finalIndex, comm, np_size, rank, next_depth, max_depth, surplus_np);

            return new_node;
        }
        
        // implement the choice for splitting point
        kpoint *mypoint = choose_splitting_point( points, myaxis, N, startIndex, finalIndex); //the splitting point
        
        // We individuate the left- and right- points with a 3-ways partition.
        // OSS. points vector is already partitioned among the direction myaxis thanks to choose_splitting_point func:
        int j= startIndex + N/2;

        int N_left = j - startIndex;
        int N_right= finalIndex - j;

        new_node->axis = myaxis; //save the splitting dimension ion the new node
        new_node->split = *mypoint; // here we save the kpoint of the splitting point on the new node

        if (right_rank == -1) { 

            #ifdef DEBUG
            printf("Process [%d]: no available processes, going single core from now\n", rank);
            #endif

            if(N_left > 0)
                new_node->left = serial_build_kdtree( points, ndim, myaxis, startIndex, j - 1);
            if(N_right > 0)
                new_node->right = serial_build_kdtree( points, ndim, myaxis, j + 1, j + N_right);
        }else{
            // Left branch continues on same process on deeper depth of the tree
            if(N_left > 0){ 

                new_node->left = build_kdtree( points, ndim, myaxis, startIndex, j - 1, comm, np_size, rank, next_depth, max_depth, surplus_np);
                
                // Set to null the right branch on this process
                kdnode * missing = malloc(sizeof(kdnode));
                missing -> axis = -1;
                missing -> left = NULL;
                missing -> right = NULL;
                new_node -> right = missing;
            }
            // Sending right branch and the other paramteres to the right process
            if(N_right > 0){

                // int startIndex_R = j + 1;
                // int finalIndex_R = j + N_right;
                kpoint * right_points = points + j + 1;

                // # Send params for build_kdtree calling on right branch
                int params[] = {N_right, ndim, myaxis, np_size, right_rank, next_depth, max_depth, surplus_np};
                MPI_Send(params, 8, MPI_INT, right_rank, rank, comm); 

                // # Send fraction of points to work on the right branch
                MPI_Send(right_points, N_right * sizeof(kpoint), MPI_BYTE, right_rank, rank, comm);
            }
        }

        return new_node;
    }

    // If k is more than the number of elements in the array
    fprintf(stderr, RED "[ERROR]"
            NC  ": N turned out to be negative! \n");
    MPI_Finalize() ;
    exit(EXIT_FAILURE);

}


kdnode *prepare_build(MPI_Comm comm){

    int params[8];
    MPI_Status status;
    kpoint * points = NULL;

    // Receive number of points N_right
    MPI_Recv(params, 8, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, comm, &status); 
    // Allocate new pointer
    points = malloc(params[0] * sizeof(kpoint)); 
    // Receive fraction of points
    MPI_Recv(points, params[0] * sizeof(kpoint), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, comm, &status); 
    
    PRINTF("MPI process %d received points from rank %d, with tag %d and error code %d.\n", 
               params[4],
               status.MPI_SOURCE,
               status.MPI_TAG,
               status.MPI_ERROR);

    // Start building the sub-kdtree
    kdnode * sub_kdtree = build_kdtree(points, params[1], params[2], 0, params[0]-1, comm, params[3], params[4], params[5], params[6], params[7]); 
    // if(PRINT_TREE){
    //         unsigned int depth = 1;
    //         printf("The nodes are the following:\n");
    //         printTree(kdtree, depth);
    // }

    return sub_kdtree;
}