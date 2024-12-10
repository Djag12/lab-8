#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "./Headers/list.h"
#include "./Headers/util.h"

/**
 * Function: TOUPPER
 * -----------------
 * Converts a string to uppercase.
 *
 * Parameters:
 *  arr: Pointer to the string to be converted.
 *
 * Description:
 *  Iterates over each character in the string, converting each to uppercase.
 */
void TOUPPER(char * arr){
  
    for(size_t i=0;i<strlen(arr);i++){
        arr[i] = toupper(arr[i]);
    }
}

/**
 * Function: get_input
 * -------------------
 * Parses the input file and determines the memory allocation policy.
 *
 * Parameters:
 *  args: Command line arguments.
 *  input: Array to store the parsed data from the file.
 *  n: Pointer to store the number of data entries parsed.
 *  size: Pointer to store the size of the initial memory partition.
 *  policy: Pointer to store the chosen memory management policy.
 *
 * Description:
 *  Opens the specified input file, parses the data, and sets the memory management policy
 *  based on command line arguments. Supports 'FIFO', 'Best Fit', and 'Worst Fit' policies.
 */
void get_input(char *args[], int input[][2], int *n, int *size, int *policy) 
{
  	FILE *input_file = fopen(args[1], "r");
	  if (!input_file) {
		    fprintf(stderr, "Error: Invalid filepath\n");
		    fflush(stdout);
		    exit(0);
	  }

    parse_file(input_file, input, n, size);
  
    fclose(input_file);
  
    TOUPPER(args[2]);
  
    if((strcmp(args[2],"-F") == 0) || (strcmp(args[2],"-FIFO") == 0))
        *policy = 1;
    else if((strcmp(args[2],"-B") == 0) || (strcmp(args[2],"-BESTFIT") == 0))
        *policy = 2;
    else if((strcmp(args[2],"-W") == 0) || (strcmp(args[2],"-WORSTFIT") == 0))
        *policy = 3;
    else {
       printf("usage: ./mmu <input file> -{F | B | W }  \n(F=FIFO | B=BESTFIT | W-WORSTFIT)\n");
       exit(1);
    }
        
}

/**
 * Function: allocate_memory
 * -------------------------
 * Allocates memory blocks based on the specified policy.
 *
 * Parameters:
 *  freelist: Pointer to the list of free memory blocks.
 *  alloclist: Pointer to the list of allocated memory blocks.
 *  pid: Process ID requesting memory allocation.
 *  blocksize: Size of the memory block to allocate.
 *  policy: Memory management policy to use for allocation.
 *
 * Description:
 *  Allocates a block of memory for the specified process ID according to the chosen policy.
 *  Supports 'First Fit', 'Best Fit', and 'Worst Fit' allocation strategies.
 */
void allocate_memory(list_t *freelist, list_t *alloclist, int pid, int blocksize, int policy) {
    node_t *current = freelist->head;
    node_t *best_fit = NULL;
    node_t *worst_fit = NULL;

    // Iterate through the free list to find a suitable block
    while (current != NULL) {
        int current_size = current->blk->end - current->blk->start + 1;
        if (current_size >= blocksize) {
            if (policy == 1) {  // First Fit
                best_fit = current;
                break;
            } else if (policy == 2) {  // Best Fit
                if (!best_fit || (current_size >= blocksize && current_size < best_fit->blk->end - best_fit->blk->start + 1)) {
                    best_fit = current;
                }
            } else if (policy == 3) {  // Worst Fit
                if (!worst_fit || current_size > worst_fit->blk->end - worst_fit->blk->start + 1) {
                    worst_fit = current;
                }
            }
        }
        current = current->next;
    }

    node_t *selected_block = (policy == 3) ? worst_fit : best_fit;

    if (selected_block) {
        // Allocate the block
        block_t *new_block = malloc(sizeof(block_t));
        *new_block = *(selected_block->blk);  // Copy block data
        new_block->pid = pid;
        new_block->end = new_block->start + blocksize - 1;

        list_add_ascending_by_address(alloclist, new_block);

        // Handle the remaining memory (fragment)
        if (new_block->end < selected_block->blk->end) {
            block_t *fragment = malloc(sizeof(block_t));
            fragment->pid = 0;  // Free block
            fragment->start = selected_block->blk->start = new_block->end + 1;
            fragment->end = selected_block->blk->end;
            list_add_to_freelist(freelist, fragment, policy);  // Add back to free list
        }

        // Remove the original block from the free list
        remove_block_from_freelist(freelist, selected_block->blk);

    } else {
        fprintf(stderr, "Error: Not Enough Memory for PID %d\n", pid);
        return; // Early return if no suitable block is found
    }
}

/**
 * Function: deallocate_memory
 * ---------------------------
 * Deallocates memory blocks based on the specified policy.
 *
 * Parameters:
 *  alloclist: Pointer to the list of allocated memory blocks.
 *  freelist: Pointer to the list of free memory blocks.
 *  pid: Process ID requesting memory deallocation.
 *  policy: Memory management policy to use for deallocation.
 *
 * Description:
 *  Deallocates a block of memory for the specified process ID according to the chosen policy.
 *  Supports 'First Fit', 'Best Fit', and 'Worst Fit' deallocation strategies.
 *  Finds and deallocates a memory block assigned to a specific process ID.
 *  The deallocated block is then added back to the free list according to the specified memory management policy.
 */
void deallocate_memory(list_t *alloclist, list_t *freelist, int pid, int policy) {
    node_t *current = alloclist->head;
    node_t *prev = NULL;

    // Find the block with the given PID
    while (current != NULL) {
        if (current->blk->pid == pid) {
            block_t *block_to_deallocate = current->blk;

            //Add the block back to the free list
            list_add_to_freelist(freelist, block_to_deallocate, policy);
            block_to_deallocate->pid = 0;  // Set PID to 0 to indicate that it is free

            // Remove the block from the allocated list
            if (prev) {
                prev->next = current->next;
            } else {
                alloclist->head = current->next;
            }

            // Safely removing the current node from the list, since its block is in the freelist
            free(current);
            alloclist->length--;
            current = NULL; //Setting pointer to NULL to avoind dangling pointers
            return; // exit after deallocation
        }
        prev = current;
        current = current->next;
    }
    fprintf(stderr, "Memory block with PID %d not found for deallocaiton\n", pid);
    return; // returning early if not found
}

/**
 * Function: coalese_memory
 * ------------------------
 * Combines adjacent free memory blocks into larger blocks.
 *
 * Parameters:
 *  list: Pointer to the list of free memory blocks.
 *
 * Returns:
 *  A new list with coalesced memory blocks.
 *
 * Description:
 *  Iterates through the given list of memory blocks and merges adjacent free blocks to form larger continuous memory spaces.
 *  This helps optimize the memory utilization and allocation process.
 */
list_t* coalese_memory(list_t * list){
  list_t *temp_list = list_alloc();
  block_t *blk;
  
  while((blk = list_remove_from_front(list)) != NULL) {  // sort the list in ascending order by address
        list_add_ascending_by_address(temp_list, blk);
  }
  
  // try to combine physically adjacent blocks
  
  list_coalese_nodes(temp_list);
        
  return temp_list;
}

/**
 * Function: print_list
 * --------------------
 * Prints the contents of a memory block list.
 *
 * Parameters:
 *  list: Pointer to the list of memory blocks to be printed.
 *  message: Description or title to be printed before listing the memory blocks.
 *
 * Description:
 *  Iterates through the list and prints details of each memory block, including its start and end addresses, and the process ID (if any).
 */
void print_list(list_t * list, char * message){
    node_t *current = list->head;
    block_t *blk;
    int i = 0;
  
    printf("%s:\n", message);
  
    while(current != NULL){
        blk = current->blk;
        printf("Block %d:\t START: %d\t END: %d", i, blk->start, blk->end);
      
        if(blk->pid != 0)
            printf("\t PID: %d\n", blk->pid);
        else  
            printf("\n");
      
        current = current->next;
        i += 1;
    }
}

/* DO NOT MODIFY */
/**
 * Function: main
 * --------------
 * Entry point for the memory management unit simulator.
 *
 * Parameters:
 *  argc: Argument count.
 *  argv: Argument vector.
 *
 * Returns:
 *  Status code (0 for successful execution).
 *
 * Description:
 *  Handles the setup and execution of the memory management unit (MMU) simulation.
 *  Processes input data for memory allocation and deallocation requests and executes these requests based on the specified policy.
 */

#ifndef TESTING
int main(int argc, char *argv[]) 
{
   int PARTITION_SIZE, inputdata[200][2], N = 0, Memory_Mgt_Policy;
  
   list_t *FREE_LIST = list_alloc();   // list that holds all free blocks (PID is always zero)
   list_t *ALLOC_LIST = list_alloc();  // list that holds all allocated blocks
   int i;
  
   if(argc != 3) {
       printf("usage: ./mmu <input file> -{F | B | W }  \n(F=FIFO | B=BESTFIT | W-WORSTFIT)\n");
       exit(1);
   }
  
   get_input(argv, inputdata, &N, &PARTITION_SIZE, &Memory_Mgt_Policy);
  
   // Allocated the initial partition of size PARTITION_SIZE
   
   block_t * partition = malloc(sizeof(block_t));   // create the partition meta data
   partition->start = 0;
   partition->end = PARTITION_SIZE + partition->start - 1;
                                   
   list_add_to_front(FREE_LIST, partition);          // add partition to free list

   // Check for empty input data
    if (N == 0) {
        fprintf(stderr, "Error: No data in input file\n");
        exit(EXIT_FAILURE);
    }
                                   
   for(i = 0; i < N; i++) // loop through all the input data and simulate a memory management policy
   {
       printf("************************\n");
       if(inputdata[i][0] != -99999 && inputdata[i][0] > 0) {
             printf("ALLOCATE: %d FROM PID: %d\n", inputdata[i][1], inputdata[i][0]);
             allocate_memory(FREE_LIST, ALLOC_LIST, inputdata[i][0], inputdata[i][1], Memory_Mgt_Policy);
       }
       else if (inputdata[i][0] != -99999 && inputdata[i][0] < 0) {
             printf("DEALLOCATE MEM: PID %d\n", abs(inputdata[i][0]));
             deallocate_memory(ALLOC_LIST, FREE_LIST, abs(inputdata[i][0]), Memory_Mgt_Policy);
       }
       else {
             printf("COALESCE/COMPACT\n");
             FREE_LIST = coalese_memory(FREE_LIST);
       }   
     
       printf("************************\n");
       print_list(FREE_LIST, "Free Memory");
       print_list(ALLOC_LIST,"\nAllocated Memory");
       printf("\n\n");
   }
  
   list_free(FREE_LIST);
   list_free(ALLOC_LIST);
  
   return 0;
}
#endif