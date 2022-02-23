#ifndef MEDOFMED_MP_H
#define MEDOFMED_MP_H

#include "kd_tree_mp.h"
#define RED "\e[0;31m"
#define NC "\e[0m"

void swap(double*, double*);
int twoWaysPartition(kpoint*, int, int, double, int);
kpoint *getMedian(kpoint*, int, int, int, int);
void insertionSort(double[], int);
void findMedian(kpoint*, int, int, double*);
void swap_kpoint(kpoint *, kpoint *);

#endif