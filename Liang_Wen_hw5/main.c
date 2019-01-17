#include "main.h"
#include <stdlib.h>
#include <string.h>
//#include "bestFit.c"
//#include "firstFit.c"
//#include "buddySys.c"
int main(int argc, char *argv[]){

 
  if(!argc > 3){
    printf("\nYou must provide the following arguments"); 
    printf("\npolicy, total memory free pool size, name of file containg requests");
    printf("\nExample: ./a.out best 1024 proj5_data > output.txt");
    
    printf("\nYou only provided %d arguements.", argc);

    exit(2);
  }

  
  int mem = atoi(argv[2]);

 
  if(strcmp(argv[1], "best") == 0){
    bestFit(mem, argv[3]);
  }
  

  if(strcmp(argv[1], "first") == 0){
     firstFit(mem, argv[3]);
  }

  
  if(strcmp(argv[1], "buddy") == 0){
    buddySys(mem, argv[3]); 
  }

  return 0;


}
