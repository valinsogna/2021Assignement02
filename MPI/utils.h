#ifndef UTILS_H
#define UTILS_H

int compute_max_depth(int n_processes);
int compute_n_surplus_processes(int n_processes, int max_depth);
int compute_next_process_rank(int rank, int max_depth, int next_depth, int surplus_processes, int n_processes);


#endif