
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {
	int    pfdOut[2], pfdIn[2];
	int     fd, numBytesRead;
	char    buffer[80];

	/* use the pipe() system call to create the pipe and check */
	
	if (pipe(pfdOut) == -1 || pipe(pfdIn) == -1) {
		perror("failed in Parent pipe creation:");
		exit(3);
	}

	switch (fork()) {

	case -1: /* call failed */
		perror("failed in Fork call  : ");
		exit(5);

	case 0:
	{
		/* child process */
		/* Close the Child's stdout and stdin */
		/* Use parents stdout/stdin as the it's own */
		if (close(0) == -1) {
			perror("CHILD : Could not close stdin.\n");
			exit(6);
		}

		if (dup(pfdOut[0]) != 0) {
			perror("CHILD : Could use parents stdin.\n");
			exit(6);
		}

		if (close(1) == -1) {
			perror("CHILD : Could not close stdout.\n");
			exit(6);
		}

		if (dup(pfdIn[1]) != 1) {
			perror("CHILD : Could not use parents stdout.\n");
			exit(6);
		}

		/* Close uneeded pipes */
		if (close(pfdOut[0]) == -1 || close(pfdOut[1]) == -1 ||
			close(pfdIn[0]) == -1 || close(pfdIn[1]) == -1) {

			perror("CHILD : One of the pipes failed to close.\n");
			exit(7);
		}

		/* Load the sort */
		execlp("grep", "grep", "123", NULL);

		/* If we reach this line the grep did not work */
		perror("Grep has failed to run correctly.\n");
		exit(4);
	}

	default: /* for the parent */
		break;

	} /* switch and child end */

	  /* Close uneeded pipes */
	if (close(pfdOut[0]) == -1 || close(pfdIn[1]) == -1) {
		printf("PARENT : Could not close pipes");
		exit(4);
	}

	if ((fd = open("cs308a2_grep_data_1", O_RDONLY, 0)) == -1) {
		perror("PARENT : Failed to open file");
		exit(-1);
	}
		int i;
		int c=0;
	/* Read the data from the file and write into the pipe */
	/* The pipe is connected to the child which is running grep */
	while ((numBytesRead = read(fd, buffer, 80)) != 0) {
		
			printf("*");
			c++;
		
		if (c%52==0)
			{	
				//printf("777");
				i=c*80;
				printf("   %d bytes sent\n",i);
			}	
		if (numBytesRead == -1) {
			printf("Write error");
			exit(3);
		}

		if (write(pfdOut[1], buffer, numBytesRead) == -1) {
			printf("PARENT : Could not write to child.\n");
			exit(2);
		
	}

	
	}
printf("\nALL DATA SENT\n");
		if (close(fd) == -1 || close(pfdOut[1]) == -1) {
		printf("PARNET : Could not close pipe or fileDesc.\n");
		exit(1);
	}
	
	

/* Create and use temporary file to store grep output*/
FILE* fp = fopen("save.txt", "w+");
      
fd = open("save.txt", O_RDWR);
	
/* Read back from grep and write to file ,screen */ 
while((numBytesRead = read(pfdIn[0], buffer, 80)) != 0){
	if (write(1, buffer, numBytesRead) == -1 || write(fd, buffer, numBytesRead) == -1)
	{	
		perror("write to screen or file failed");
		exit(6);
	}
}

/* Count lines in temporary file */
int count = 0;
while(fgets(buffer, 80, fp) != NULL){
  count++;	
}

	

printf("Total matching line count: %d \n", count); 
close(pfdIn[0]);
return 0; 
}

