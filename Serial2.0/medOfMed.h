#ifndef MEDOFMED_H
#define MEDOFMED_H

#include "kd_tree.h"
#define RED "\e[0;31m"
#define NC "\e[0m"

void swap(double*, double*);
int twoWaysPartition(kpoint*, int, int, double, short int);
kpoint *getMedian(kpoint*, int, int, int, short int);
void insertionSort(double[], int);
void findMedian(kpoint*, int, short int, double*);
void swap_kpoint(kpoint *, kpoint *);

#endif