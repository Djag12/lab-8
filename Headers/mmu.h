#ifndef MAIN_H
#define MAIN_H

#include "list.h"  // Include the header for list-related definitions

// Function prototypes
void get_input(char *args[], int input[][2], int *n, int *size, int *policy);
void allocate_memory(list_t *freelist, list_t *alloclist, int pid, int blocksize, int policy);
void deallocate_memory(list_t *alloclist, list_t *freelist, int pid, int policy);
list_t* coalese_memory(list_t *list);
void print_list(list_t *list, char *message);

#endif // MAIN_H
