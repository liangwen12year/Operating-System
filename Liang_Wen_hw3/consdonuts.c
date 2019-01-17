#include <stdio.h>
#include "donuts.h"
int shmid, semid[3];

int main(int argc,char *argv[])
{
	int i, j, k, donut,s;
        int count1=0;
	int count2=0;
	int count3=0;
	int count4=0;
	int jelly[12]={};
	int coconut[12]={};
	int plain[12]={};
	int glazed[12]={};
	
	struct tm* ptm; 
	char szTime[40];
  	struct donut_ring *shared_ring;
  	struct timeval randtime;
	unsigned short xsub1[3];
	char *dtype[] = {"jelly" , "coconut", "plain", "glazed"};
	key_t memkey, semkey;
	
	//memkey = semkey = MEMKEY + getuid();

	if((shmid=shmget(MEMKEY, sizeof(struct donut_ring), 0))==-1){
		perror("shared get failed:");
		exit(1);
	}

	if((shared_ring=(struct donut_ring*)shmat(shmid, NULL,0))==(void*)-1){
		perror("shared attach failed:");
		exit(1);	
	}
	
	for(i=0; i<NUMSEMIDS; i++){
		if((semid[i]=semget(SEMKEY+i, NUMFLAVORS, 0)) == -1){
			perror("semaphore allocation failed:");
			exit(1);	
		}		
	}
	
	gettimeofday(&randtime, NULL);

	xsub1[0] = (ushort)randtime.tv_usec;
	xsub1[1] = (ushort)(randtime.tv_usec >> 16);
	xsub1[2] = (ushort)(getpid());

	 /* Time formatting */
	ptm = localtime(&randtime.tv_sec);

    	strftime(szTime, sizeof(szTime), "%H:%M:%S", ptm);
    	  int ms = randtime.tv_usec/1000 ;
	printf("consumer %s consume dunuts \n\n",argv[1]);
	for(i=0;i<10;i++){  
	printf("\nProcess PID: %d\t\t", getpid());
    	printf("Dozen number: %d\t\t", i + 1);
	printf("Time: %s.%d\n", szTime, ms);
	printf("jelly\t\tcoconut\t\tplain\t\tglazed\t\t\n");

	//printf("\nTime: %s\n", gettimeofday(&randtime, (struct timezone *)0));
	//gettimeofday(&randtime, (struct timezone *)0)
		for(k=0;k<12;k++){
			j=nrand48(xsub1) & 3;	
		
			if( p(semid[CONSUMER], j) == -1){
				perror("p operation failed:");
				exit(3);
			}
			if( p(semid[OUTPTR], j) == -1){
				perror("p operation failed:");
				exit(3);
			}
			donut=shared_ring->flavor[j][shared_ring->outptr[j]];
			shared_ring->outptr[j] = (shared_ring->outptr[j]+1) % NUMSLOTS;
			if( v(semid[PROD], j) == -1){
				perror("v operation failed:");
				exit(3);		
			}
			if( v(semid[OUTPTR], j) == -1){
				perror("v operation failed:");
				exit(3);
			}			

			//printf("donut type %s, \tserial number %d\n",dtype[j],donut);
			if(dtype[j]=="jelly"){
				jelly[count1]=donut;
				count1++;
				}
			 if(dtype[j]=="coconut"){
				coconut[count2]=donut;
				count2++;
				}
			 if(dtype[j]=="plain"){
				plain[count3]=donut;
				count3++;
				}	
			 if(dtype[j]=="glazed") {
				//printf("aaaa\n");
				glazed[count4]=donut;
				count4++;
				}
		
	}
		for(s=0;s<12;s++){
			printf("%d\t\t%d\t\t%d\t\t%d\t\t\n",jelly[s],coconut[s],plain[s],glazed[s]);}
			//printf("***********\n");
		//printf("consumer %s finished dozen number %d \n\n",argv[1],i);
		usleep(100);
		for(s=0;s<12;s++){
			jelly[s]=0;
			coconut[s]=0;
			plain[s]=0;
			glazed[s]=0;
			}
			count1=0;
			count2=0;
			count3=0;
			count4=0;

		}
		fprintf(stderr, "	CONSUMER %s DONE\n", argv[1]);
	return 0;
}			

