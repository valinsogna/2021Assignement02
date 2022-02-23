#ifndef MEDOFMED_H
#define MEDOFMED_H
//
#include "kd_tree.h"
#define RED "\e[0;31m"
#define NC "\e[0m"

void swap(double*, double*);
int twoWaysPartition(struct kpoint*, int, int, double, int);
struct kpoint *getMedian(struct kpoint*, int, int, int, int);
void insertionSort(double[], int);
void findMedian(struct kpoint*, int, int, double*);
void swap_kpoint(struct kpoint *, struct kpoint *);

#endif