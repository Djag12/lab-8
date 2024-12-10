#define _GNU_SOURCE
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>

#include "./Headers/util.h"
#include "./Headers/list.h"

void parse_file(FILE *f, int input[][2], int *n, int *PARTITION_SIZE) {
    if (f == NULL) {
        fprintf(stderr, "Error: File pointer is NULL\n");
        return;
    }

    // Get the initial partition size
    if (fscanf(f, "%d\n", PARTITION_SIZE) != 1) {
        fprintf(stderr, "Error reading partition size\n");
        return;
    }
    printf("PARTITION_SIZE = %d\n", *PARTITION_SIZE);

    // Read the rest of the file
    while (1) {
        int read = fscanf(f, "%d %d\n", &input[*n][0], &input[*n][1]);
        if (read != 2) {
            if (feof(f)) {
                // Reached the end of the file
                break;
            } else {
                // Read error occurred
                fprintf(stderr, "Error reading file\n");
                break;
            }
        }
        *n += 1;
    }
}