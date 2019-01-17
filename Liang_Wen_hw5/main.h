#ifndef _MAIN_H
#define _MAIN_H

#include<stdio.h>
#include<math.h>

#define NUMBER_ENTRIES  (1001)
#define FALSE           (0)
#define TRUE            (1)
#define DONE            (2)

struct request{
        int is_req;
        int is_allocated;
        int size;
        int base_adr;
        int next_boundary_adr;
        int memory_left;
        int largest_chunk;
        int elements_on_free_list;
}req_array[NUMBER_ENTRIES];

struct free_list{
        struct free_list *next;
        struct free_list *previous;
        int block_size;
        int block_adr;
        int adjacent_adr;
}list_head, *top;

int total_free_space;
//int free_list_length = 0;
int total_free;

int allocateMemoryBest(struct request *);
int allocateMemoryFirst(struct request *);
int allocateMemoryBuddySys(struct request *);

int update_list(int);

void print_results(char* policy, int memorySize, struct request* req);
int bestFit(int memorySize, char* fileToWrite);
int buddySys(int memorySize, char* fileToWrite);
int firstFit(int memorySize, char* fileToWrite);
/*
// the buddy system requires a more complex set of structures 
// as suggested below

// a block list element on one of the block sized
// list of addresses
struct lel{
        struct lel *next;
        struct lel *prev;
        int adr;
        int bud_adr;
        int bit;
};

// a list head for one of the block sized lists that hold
// struct lel elements (need one for each in-range power of 2)
struct lh{
        struct lel *head;
        int cnt;
};

struct lh lst_ary[21];
int mal_cnt=0;

struct lel *find_el(int bit);
int update_lists(struct lel *el);
*/ 
#endif
