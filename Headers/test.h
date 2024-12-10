// test.h
#ifndef TEST_H
#define TEST_H

#include "list.h"

// Prototypes for test functions
void run_all_tests();
void test_allocate_memory();
void test_deallocate_memory();
void test_coalese_memory();
void test_list_add_to_freelist();
void test_remove_block_from_freelist();
void test_allocate_memory_edge_cases();
void test_deallocate_memory_edge_cases();

#endif /* TEST_H */
