

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
int main(void){
	int     pfdOut[2], pfdIn[2]; 

	/* use the pipe() system call to create the pipe and check */
        
        if(pipe(pfdOut) == -1 || pipe(pfdIn) == -1){
                perror("failed in Parent pipe creation:");
                exit(7);
        }
	
	switch(fork()){

	case -1: /* call failed */
	  perror("failed in the fork call: ");
	  exit(3);

	case 0 : /* child process */
	  /* Close the Child's stdout and stdin */
          /* Use parents stdout/stdin as the it's own */
	  if (close(0) == -1) {
	    perror("CHILD : Could not close stdin.\n");
            exit(7);
	  }

          if (dup(pfdOut[0]) != 0) {
            perror("CHILD : Could not use parents stdin.\n");
            exit(7);
          }

          if (close(1) == -1) {
            perror("CHILD : Could not close stdout.\n");
            exit(7);
          }

          if (dup(pfdIn[1]) != 1) {
            perror("CHILD : Could not use parents stdout.\n");
            exit(7);
          }

	  /* Close unneeded pipes */
	  if ( close(pfdOut[0]) == -1 || close(pfdOut[1]) == -1 ||
	       close(pfdIn[0]) == -1 || close(pfdIn[1]) == -1) {
	    
	    perror("CHILD : One of the pipes failed to close.\n");
	    exit(1);
	  }

	  /* Load the sort */
	  execlp("sort", "sort", "-k", "3.3n", "-k", "1.1", "-k", "2.2", NULL);

	  /* If we reach this line the sort did not work */ 
	  perror("failed in running sort in the child process.\n");
	  exit(3);

        default : /* for the parent */
	  break; 

        } /* switch and child end */

	/* Close unneeded pipes */
	if(close(pfdOut[0]) == -1 || close(pfdIn[1]) == -1){
	  printf("PARENT : Could not close pipes");
	  exit(6);
	}

	int fd = 0;
        fd = open("cs308a2_sort_data", O_RDONLY, 0);

	/* Read the data from the file and write into the pipe */
	/* The pipe is connected to the child which is running sort */
	char buffer[512];
	int numBytesRead;
		int i;
		int c=0;
	while(numBytesRead = read(fd, buffer, 80)){
			printf("*");
			c++;
		
				if (c%52==0)
			{	
				//printf("777");
				i=c*80;
				printf("   %d bytes sent\n",i);
			}	
	  if(write(pfdOut[1], buffer, numBytesRead) == -1){
	    printf("PARENT : Could not write to child.\n");
	    exit(2);
	  }     
	
	}
printf("\nALL DATA SENT\n");
	if(close(pfdOut[1]) == -1 || close(fd)==-1){
	  printf("PARNET : Could not close pipe and fd.\n");
	  exit(1);
	}

	/* Open patent's stdin and read */ 
	FILE* fp = fdopen(pfdIn[0], "r"); 
 	
	/* Number of area codes of the same number */
	int areaCodeCount = 0;

	/* Old Area code number */
	int oldAreaCode = 0; 

	char firstName[100], lastName[100];
	int areaCode, prefix, tel;
	
	int sum=0;
	while(fscanf(fp, "%s %s %d %d %d\n", lastName, firstName, &areaCode, &prefix, &tel) != EOF){
	  
	  if (oldAreaCode == 0) {
	    /* First run of this loop */
	    oldAreaCode = areaCode;
	  } 

	  if (oldAreaCode == areaCode){
	    areaCodeCount++;
	    continue; 
	  } else { 
	    /* New area codes */
	    /* Print out data on the previous code and reset */ 
	    printf("area code %d occurs: %d times\n", oldAreaCode, areaCodeCount); 
	    oldAreaCode = areaCode;
	    sum=sum+areaCodeCount;
	    areaCodeCount = 1;
	  }

	}


	/* Print remaining area code info, since the loop finishes leaving it unprinted */
	printf("area code %d occurs: %d times\n", oldAreaCode, areaCodeCount);
	sum=sum+areaCodeCount;
	printf("%d RECORDS PROCESSED ... GOODBYE\n", sum);

	fclose(fp);

	return 0; 

}
