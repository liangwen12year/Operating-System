#include "main.h"
#include <stdlib.h>
#include <string.h>
int allocateMemoryBuddySys(struct request* request){

  struct free_list* freeList = NULL;
  struct free_list* validList = NULL;

  int sizeDiff = 0;
  int leftover = 0;


  for(freeList = list_head.next; freeList; freeList = freeList -> next){
    if(request -> size <= freeList -> block_size){
      sizeDiff = freeList -> block_size - request -> size;

      /* found smaller chunk */ 
      if (leftover >= sizeDiff){
	leftover = sizeDiff;
	validList = freeList; 
      }

    }
  }


  if(validList != NULL){
    /* Set request */
    request -> is_allocated = TRUE;
    request -> base_adr = validList -> block_adr;
    request -> next_boundary_adr = request -> base_adr + request -> size;

    /* update remaining space */
    total_free = total_free - request -> size;
    request -> memory_left = total_free;


 
    if((validList->block_size = validList->block_size - request -> size) == 0)
      {
	validList -> previous -> next = validList -> next;
	validList -> next -> previous = validList -> previous;
	free(validList);
	return 0;
      }

    validList -> block_adr = validList -> block_adr + request -> size;
    return 0;

  }

  /* Did not find a valid space */ 
  request -> memory_left = total_free;
  return 0;

}

int buddySys(int memorySize, char* fileToWrite){

  int i = 0;
  int requestSeq = 0;
  int requestSize = 0;
  
  char requestType[30];


  FILE* file = fopen(fileToWrite, "r");

  
  struct free_list* freeList; 

   
  total_free_space = total_free = (memorySize * 1024);


  for(i = 0; i < NUMBER_ENTRIES; i++){
    req_array[i].is_req = FALSE;
    req_array[i].is_allocated = FALSE;
  }


  top = malloc(sizeof(struct free_list));

  top -> next = NULL;
  top -> previous = &list_head;
  top -> block_size = total_free_space;
  top -> block_adr = 0;
  top -> adjacent_adr = total_free_space;

  /* set the list to the first free node */
  list_head.next = top;
  list_head.previous = NULL;
  list_head.block_size = -1;


  while(fscanf(file, "%d %s %d", &requestSeq, requestType, &requestSize) != EOF){

    
    if(strcmp(requestType, "alloc") == 0 ){
      
      
      req_array[requestSeq].is_req = TRUE;
      req_array[requestSeq].size = requestSize;

      allocateMemoryBuddySys(&req_array[requestSeq]);

    
      req_array[requestSeq].elements_on_free_list = 0;
      req_array[requestSeq].largest_chunk = 0;

      for(freeList = list_head.next; freeList; freeList = freeList -> next){
	
	++req_array[requestSeq].elements_on_free_list;

	
	if(freeList -> block_size > req_array[requestSeq].largest_chunk){
	  req_array[requestSeq].largest_chunk = freeList -> block_size;
	}
      }

    } else {
      /* Free operation */
      req_array[requestSeq].size = req_array[requestSize].size;
      req_array[requestSeq].is_allocated = req_array[requestSize].is_allocated;

      update_list(requestSize);

     
      req_array[requestSeq].memory_left = total_free;
      req_array[requestSeq].elements_on_free_list = 0;
      req_array[requestSeq].largest_chunk = 0;

      for(freeList = list_head.next; freeList; freeList = freeList -> next){
        
        ++req_array[requestSeq].elements_on_free_list;

       
        if(freeList -> block_size > req_array[requestSeq].largest_chunk){
          req_array[requestSeq].largest_chunk = freeList -> block_size;
        }
      }

    }

  }

  print_results("Buddy System", memorySize, req_array);

  /* All set close it out */ 
  fclose(file);

}
