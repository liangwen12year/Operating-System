
#include "main.h"
#include <stdlib.h>
int update_list(int index){

 
  struct free_list* freeList;
  struct free_list* newBlock;  
  struct free_list* combineBlock;
 
  if(req_array[index].is_allocated == FALSE){
   
    printf("Request %d Failed\n", index);
    return 0;
  }

  /* block is done */
  req_array[index].is_allocated = DONE;
  total_free += req_array[index].size;

 
  for(freeList = list_head.next; freeList; freeList = freeList -> next){

   
    if(req_array[index].base_adr > freeList -> block_adr) {
      /* skip */
      continue;
    }

    
    newBlock = malloc(sizeof(struct free_list));    
    newBlock -> block_size = req_array[index].size;
    newBlock -> block_adr = req_array[index].base_adr;
    newBlock -> adjacent_adr = newBlock -> block_adr + newBlock -> block_size;

    newBlock->next = freeList;
    freeList -> previous -> next = newBlock;
    newBlock -> previous = freeList -> previous;
    freeList -> previous = newBlock;
    
 
    if(newBlock -> adjacent_adr == newBlock -> next -> block_adr) {
      combineBlock = newBlock -> next;
      newBlock -> block_size = newBlock -> block_size + newBlock -> next -> block_size;
      newBlock -> adjacent_adr = newBlock -> next -> adjacent_adr;
      newBlock -> next = newBlock -> next -> next;
      
      if(newBlock -> next){
        newBlock -> next -> previous = newBlock;
      }

      free(combineBlock);

    }

    /* with previous */ 
    newBlock = newBlock -> previous;
    if((newBlock != NULL) && (newBlock -> adjacent_adr != 0)) {
      if(newBlock -> adjacent_adr == newBlock -> next -> block_adr) {
        combineBlock = newBlock->next;
        newBlock -> block_size = newBlock -> block_size + newBlock -> next -> block_size;
        newBlock -> adjacent_adr = newBlock -> next -> adjacent_adr;
        newBlock -> next = newBlock -> next -> next;

        if(newBlock -> next){
          newBlock ->next -> previous = newBlock;
	}

        free(combineBlock);

      }
    }

    return 0;

  }
}

void print_results(char* policy, int memorySize, struct request* req){

  /* Headers */
  printf("\nPOLICY: %s\tMEMORY SIZE: %d\n\n", policy, memorySize);
  printf("\n\tNUMBER \tREQUEST SIZE   \tALOC-ADDR \tMEMORY LEFT \tLARGEST CHUNK\n");

  int i = 1;
  int fails = 0;
  char operation[6];

  for(i; i < NUMBER_ENTRIES; i++){

    if(req[i].is_allocated == FALSE){
      /* set to invalid address */
      req[i].base_adr = -1;

      /* keep track of how many fails */
      fails++;
    }

    /* operation type */
    if(req[i].is_req == 1){
      sprintf(operation, "%s", "alloc");
    } else {
      sprintf(operation, "%s", "free");
    }

    /* data about the request */ 
    printf("\t%d \t%s \t%d \t%-8d \t%-8d \t%-8d\n", i, operation, req[i].size,
	   req[i].base_adr, req[i].memory_left, req[i].largest_chunk);

  }

  /* bad request */ 
  printf("\n%d Allocations Successed\n\n",(500-fails/2));

}
