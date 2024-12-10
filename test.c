// test.c
#include "./Headers/test.h"
#include "./Headers/mmu.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void run_all_tests() {
    test_allocate_memory();
    test_deallocate_memory();
    test_coalese_memory();
    test_list_add_to_freelist();
    test_remove_block_from_freelist();
    test_allocate_memory_edge_cases();
    test_deallocate_memory_edge_cases();
    printf("All tests passed.\n");
}

void test_allocate_memory() {
    list_t *freelist = list_alloc();
    list_t *alloclist = list_alloc();
    int policy = 1; // Testing with FIFO policy

    // Initial conditions
    block_t *initial_block = malloc(sizeof(block_t));
    initial_block->pid = 0;
    initial_block->start = 0;
    initial_block->end = 1000;
    list_add_to_back(freelist, initial_block);

    // Test allocation
    allocate_memory(freelist, alloclist, 1, 500, policy);
    assert(alloclist->length == 1);
    assert(freelist->length == 1);

    // Clean up
    list_free(freelist);
    list_free(alloclist);
    printf("test_allocate_memory passed.\n");
}

void test_deallocate_memory() {
    list_t *freelist = list_alloc();
    list_t *alloclist = list_alloc();
    int policy = 1; // Testing with FIFO policy

    // Setup allocated block
    block_t *allocated_block = malloc(sizeof(block_t));
    allocated_block->pid = 1;
    allocated_block->start = 0;
    allocated_block->end = 499;
    list_add_to_back(alloclist, allocated_block);

    // Test deallocation
    deallocate_memory(alloclist, freelist, 1, policy);
    assert(alloclist->length == 0);
    assert(freelist->length == 1);

    // Clean up
    list_free(freelist);
    list_free(alloclist);
    printf("test_deallocate_memory passed.\n");
}

void test_coalese_memory() {
    list_t *list = list_alloc();

    // Add adjacent blocks
    block_t *block1 = malloc(sizeof(block_t));
    block1->pid = 0; block1->start = 0; block1->end = 499;
    block_t *block2 = malloc(sizeof(block_t));
    block2->pid = 0; block2->start = 500; block2->end = 999;
    list_add_to_back(list, block1);
    list_add_to_back(list, block2);

    // Test coalesce
    list_t *coalesced_list = coalese_memory(list);
    assert(coalesced_list->length == 1);
    assert(coalesced_list->head->blk->start == 0 && coalesced_list->head->blk->end == 999);

    // Clean up
    list_free(coalesced_list);
    printf("test_coalese_memory passed.\n");
}

void test_list_add_to_freelist() {
    list_t *freelist = list_alloc();
    block_t *block = malloc(sizeof(block_t));
    block->pid = 0; block->start = 0; block->end = 499;

    // Test adding to free list
    list_add_to_freelist(freelist, block, 1); // Using FIFO policy
    assert(freelist->length == 1);
    assert(freelist->head->blk == block);

    // Clean up
    list_free(freelist);
    printf("test_list_add_to_freelist passed.\n");
}

void test_remove_block_from_freelist() {
    list_t *freelist = list_alloc();
    block_t *block = malloc(sizeof(block_t));
    block->pid = 0; block->start = 0; block->end = 499;
    list_add_to_back(freelist, block);

    // Test removal from free list
    remove_block_from_freelist(freelist, block);
    assert(freelist->length == 0);

    // Clean up
    list_free(freelist);
    printf("test_remove_block_from_freelist passed.\n");
}

void test_allocate_memory_edge_cases() {
    // Test allocation with empty free list
    list_t *empty_freelist = list_alloc();
    list_t *alloc_list = list_alloc();
    allocate_memory(empty_freelist, alloc_list, 1, 500, 1); // FIFO policy
    assert(alloc_list->length == 0 && "Allocation should fail with empty free list");

    // Test allocation with insufficient block size
    list_t *freelist = list_alloc();
    block_t *small_block = malloc(sizeof(block_t));
    small_block->pid = 0; small_block->start = 0; small_block->end = 100;
    list_add_to_back(freelist, small_block);
    allocate_memory(freelist, alloc_list, 1, 500, 1); // FIFO policy
    assert(alloc_list->length == 0 && "Allocation should fail with insufficient block size");

    // Cleanup
    list_free(empty_freelist);
    list_free(freelist);
    list_free(alloc_list);
    printf("test_allocate_memory_edge_cases passed.\n");
}

void test_deallocate_memory_edge_cases() {
    // Test deallocation with non-existent PID
    list_t *freelist = list_alloc();
    list_t *alloclist = list_alloc();
    deallocate_memory(alloclist, freelist, 999, 1); // Non-existent PID
    assert(freelist->length == 0 && "Deallocation should fail with non-existent PID");

    // Cleanup
    list_free(freelist);
    list_free(alloclist);
    printf("test_deallocate_memory_edge_cases passed.\n");
}

int main() {
    run_all_tests();
    return 0;
}
