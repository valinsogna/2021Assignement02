// C implementation of median of medians algorithm with 2-ways partition
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <time.h> //uncomment for testing function!
#include "medOfMed.h"
#include "kd_tree.h"

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))
/* TEST THE FUNCTION getMedian();
#define MAX 25

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

int main()
{
    int ndim = NDIM, n=11;

    struct kpoint *arr = genRandomKPoints(ndim, n);

    printf("Array has size %d and is composed by:\n", n);
    for(int j = 0; j < n; j++) {
        printf("(%.2f,%.2f)\t", arr[j].coord[0], arr[j].coord[1]);
    }
    printf("\n");

    //Get median position index for odd/even arrays
    int k = n/2;

    double midpoint = getMedian(arr, 0, n-1, k, 0);
    //threeWaysPartition(arr, 0, n-1, 7.3);

    printf("Array at the end:\n");
    for(int j = 0; j < n; j++) {
        printf("(%.2f,%.2f)\t", arr[j].coord[0], arr[j].coord[1]);
    }
    printf("\n");

    printf("The median is %.2f\n", midpoint);
    return 0;
}
*/

// This function returns the index of the median in arr[l,...,r] using
// QuickSort 2 ways partition based method.
struct kpoint* getMedian(struct kpoint *arr, int l, int r, int k, int axis)
{
    // If k is smaller than number of elements in array
    if (k >= 0)//
    {
        int N = r-l+1; // Number of elements in arr[l..r]
 
        // Divide arr[] in groups of size 5, calculate median
        // of every group and store it in medians[] array.
        int i;
        struct kpoint* medians;
        if ( (medians = (struct kpoint*)calloc( (N+4)/5, sizeof(struct kpoint) )) == NULL ) // There will be floor((n+4)/5) groups
        {
            fprintf(stderr, RED "[ERROR]"
            NC  "I'm sorry, there is not enough memory to host %lu bytes\n",
	        (N+4)/5 * sizeof(struct kpoint) );
            exit(EXIT_FAILURE);
        }
        double midpoints_coords[NDIM];
        for (i=0; i<N/5; i++){
            findMedian(arr+l+i*5, 5, axis, midpoints_coords);
            medians[i].coord[axis] = midpoints_coords[axis];
            medians[i].coord[(axis+1)%NDIM] = midpoints_coords[(axis+1)%NDIM];
        }
        if (i*5 < N) //For last group (i=N/5) with less than 5 elements
        {
            findMedian(arr+l+i*5, N%5, axis, midpoints_coords);
            medians[i].coord[axis] = midpoints_coords[axis];
            medians[i].coord[(axis+1)%NDIM] = midpoints_coords[(axis+1)%NDIM];
            i++;
        }   
 
        // Find median of all medians using recursive call.
        // If median[] has only one element, then no need
        // of recursive call
        struct kpoint *medOfMed = (i == 1)? (medians+i-1): // case in which the arr has 1 elem
                                 getMedian(medians, 0, i-1, i/2, axis);
 
        // Partition the array and
        // get position of pivot element in sorted array
        int pos = threeWaysPartition(arr, l, r, medOfMed->coord[axis], axis);
 
 
        // If position is same as k
        if ( pos-l == k ){
            free(medians);
            return (arr + pos);
        }
            
        if ( pos-l > k ) // If position is more, recur for left subarray
            return getMedian(arr, l, pos-1, k, axis);
 
        // Else recur for right subarray
        return getMedian(arr, pos+1, r, k-pos+l-1, axis);
    }
 
    // If k is more than the number of elements in the array
    fprintf(stderr, RED "[ERROR]"
            NC  ": median position turned out to be negative! \n");
    exit(EXIT_FAILURE);
    
}
 
void swap(double *a, double *b)
{
    double temp = *a;
    *a = *b;
    *b = temp;
}

void swap_kpoint(struct kpoint *a, struct kpoint *b)
{
    struct kpoint temp = *a;
    *a = *b;
    *b = temp;
}
 
// It searches for x in arr[l..r], and partitions the array
// around x.
int threeWaysPartition(struct kpoint *arr, int l, int r, double x, int axis)
{
    // Search for x in arr[l..r] and move it to end
    int i;
    for (i=l; i<r; i++)
        if (arr[i].coord[axis] == x)
           break;
    swap_kpoint(&arr[i], &arr[r]);
 
    // Standard partition algorithm
    i = l;
    for (int j = l; j <= r - 1; j++)
    {
        if (arr[j].coord[axis] <= x)
        {
            swap_kpoint(&arr[i], &arr[j]);
            i++;
        }
        
    }

    swap_kpoint(&arr[i], &arr[r]);
    

    return i;
}

void insertionSort(double array[], int size) {
  for (int step = 1; step < size; step++) {
    double key = array[step];
    int j = step - 1;
    // Compare key with each element on the left of it until an element smaller than
    // it is found.
    // For descending order, change key<array[j] to key>array[j].
    while (key < array[j] && j >= 0) {
      array[j + 1] = array[j];
      --j;
    }
    array[j + 1] = key;
  }
}

// A simple function to find median of arr[].  This is called
// only for an array of size 5 in this program.
void findMedian(struct kpoint *arr, int n, int axis, double *midpoint_coords)
{
    double *array;

    if ( (array = (double*)calloc( n, sizeof(double) )) == NULL)
    {
        fprintf(stderr, RED "[ERROR]"
            NC  "I'm sorry, there is not enough memory to host %lu bytes\n",
	     n * sizeof(double) );
        exit(EXIT_FAILURE);
    }

    for (int i=0; i<n; i++){
        array[i] = arr[i].coord[axis]; //*(arr+sizeof(struct kpoint)*i)->coord[axis];
        //printf("array[%d]: %.2f\t", i, array[i]);
        //printf("arr[%d].coord[%d]: %.2f\n", i, axis, arr[i].coord[axis]);
    }
        
    insertionSort(array, n);// Sort the array
    midpoint_coords[axis] = array[n/2]; // Return middle element

    int j=0;
    int notFound=1;
    while (j<n && notFound){
        //printf("midpoint_coords[%d]: %.2f\t", axis, midpoint_coords[axis]);
        //printf("arr[%d].coord[%d]: %.2f\n", j, axis, arr[j].coord[axis]);
        midpoint_coords[axis] == arr[j].coord[axis] ? notFound = 0 : j++;
    }
    free(array);
    //Save vector
    midpoint_coords[(axis+1)%NDIM] = arr[j].coord[(axis+1)%NDIM];

    return;
}