#ifndef UTILS_H
#define UTILS_H
#define RED "\e[0;31m"
#define NC "\e[0m"

int compute_max_depth(int n_processes);
int compute_n_surplus_processes(int n_processes, int max_depth);
int compute_next_process_rank(int rank, int max_depth, int next_depth, int surplus_processes, int n_processes);
short int choose_splitting_dimension(kpoint*, int, short int, int, int);
double getExtent(kpoint*, int, int, int);
kpoint *choose_splitting_point(kpoint *, short int, int, int, int);
void swap_kpoint(kpoint*, kpoint*);
double find_median(kpoint*, int, short int, int, int);
int three_way_partition(kpoint*, double , short int, int, int );
void hybrid_quick_sort(kpoint *arr, int low, int high, short int axis);
void quick_sort(kpoint *arr, int low,int high, short int axis);
int partition(kpoint *arr, int low, int high, short int axis);
void insertion_sort(kpoint *arr, int low, int n, short int axis);

#endif