


#include "pc_threads.h"
#include <stdio.h>
#include <stdlib.h>

/* Global Data */
struct donut_ring shared_ring;
pthread_mutex_t prod [NUMFLAVORS];
pthread_mutex_t cons [NUMFLAVORS];
pthread_cond_t prod_cond [NUMFLAVORS];
pthread_cond_t cons_cond [NUMFLAVORS];
pthread_t thread_id [NUMCONSUMERS + 1], sig_wait_id;

/* Thread Count */
int threadNum = 0;

int  main ( int argc, char *argv[] )
{
  int i, j, k, nsigs;
  struct timeval randtime, first_time, last_time;
  struct sigaction new_act;
  int arg_array[NUMCONSUMERS];
  sigset_t  all_signals;
  int sigs[] = { SIGBUS, SIGSEGV, SIGFPE };

  pthread_attr_t thread_attr;
  struct sched_param  sched_struct;

  /**********************************************************************/
  /* INITIAL TIMESTAMP VALUE FOR PERFORMANCE MEASURE                    */
  /**********************************************************************/

  gettimeofday (&first_time, (struct timezone *) 0 );
  for ( i = 0; i < NUMCONSUMERS + 1; i++ ) {
    arg_array [i] = i; /** SET ARRAY OF ARGUMENT VALUES **/
  }

  /**********************************************************************/
  /* GENERAL PTHREAD MUTEX AND CONDITION INIT AND GLOBAL INIT           */
  /**********************************************************************/

  for ( i = 0; i < NUMFLAVORS; i++ ) {
    pthread_mutex_init ( &prod [i], NULL );
    pthread_mutex_init ( &cons [i], NULL );
    pthread_cond_init ( &prod_cond [i],  NULL );
    pthread_cond_init ( &cons_cond [i],  NULL );
    shared_ring.outptr [i]= 0;
    shared_ring.in_ptr [i]= 0;
    shared_ring.serial [i]= 0;
    shared_ring.spaces [i]= NUMSLOTS;
    shared_ring.donuts [i]= 0;
  }


  /**********************************************************************/
  /* SETUP FOR MANAGING THE SIGTERM SIGNAL, BLOCK ALL SIGNALS           */
  /**********************************************************************/

  sigfillset (&all_signals );
  nsigs = sizeof ( sigs ) / sizeof ( int );
  for ( i = 0; i < nsigs; i++ ){
    sigdelset ( &all_signals, sigs [i] );
  }
  sigprocmask ( SIG_BLOCK, &all_signals, NULL );
  sigfillset (&all_signals );
  for( i = 0; i <  nsigs; i++ ) {
    new_act.sa_handler = sig_handler;
    new_act.sa_mask = all_signals;
    new_act.sa_flags = 0;
    if ( sigaction ( sigs[i], &new_act, NULL ) == -1 ){
      perror("Cannot set signals: ");
      exit(1);
    }
  }

  printf ( "Just before threads created\n" );


  /*********************************************************************/
  /* CREATE SIGNAL HANDLER THREAD, PRODUCER AND CONSUMERS              */
  /*********************************************************************/

  if ( pthread_create (&sig_wait_id, NULL,
		       sig_waiter, NULL) != 0 ){

    printf ( "Pthread_create failed " );
    exit ( 3 );
  }

  pthread_attr_init ( &thread_attr );
  pthread_attr_setinheritsched ( &thread_attr,
				 PTHREAD_INHERIT_SCHED );

#ifdef  GLOBAL
  sched_struct.sched_priority = sched_get_priority_max(SCHED_OTHER);
  pthread_attr_setinheritsched ( &thread_attr,
				 PTHREAD_EXPLICIT_SCHED );
  pthread_attr_setschedpolicy ( &thread_attr, SCHED_OTHER );
  pthread_attr_setschedparam ( &thread_attr, &sched_struct );  
  pthread_attr_setscope ( &thread_attr,
			  PTHREAD_SCOPE_SYSTEM );
#endif

  if ( pthread_create (&thread_id[0], &thread_attr,
		       producer, NULL ) != 0 ) {
    printf ( "pthread_create failed " );
    exit ( 3 );
  }

  for ( i = 1; i < NUMCONSUMERS + 1; i++ ) {
    if ( pthread_create ( &thread_id [i], &thread_attr,
			  consumer, ( void * )&arg_array [i]) != 0 ){
      printf ( "pthread_create failed" );
      exit ( 3 );
    }
  }

  printf ( "Just after threads created\n" );

  /*********************************************************************/
  /* WAIT FOR ALL CONSUMERS TO FINISH, SIGNAL WAITER WILL              */
  /* NOT FINISH UNLESS A SIGTERM ARRIVES AND WILL THEN EXIT            */
  /* THE ENTIRE PROCESS....OTHERWISE MAIN THREAD WILL EXIT             */
  /* THE PROCESS WHEN ALL CONSUMERS ARE FINISHED                       */
  /*********************************************************************/

  for ( i = 1; i < NUMCONSUMERS + 1; i++ ) 
    pthread_join ( thread_id [i], NULL ); 

  /*****************************************************************/
  /* GET FINAL TIMESTAMP, CALCULATE ELAPSED SEC AND USEC           */
  /*****************************************************************/

  gettimeofday (&last_time, ( struct timezone * ) 0 );
  if ( ( i = last_time.tv_sec - first_time.tv_sec) == 0 )
    j = last_time.tv_usec - first_time.tv_usec;
  else{
    if ( last_time.tv_usec - first_time.tv_usec < 0 ) {
      i--;
      j = 1000000 + 
	( last_time.tv_usec - first_time.tv_usec );
    } else {
      j = last_time.tv_usec - first_time.tv_usec; }
  }
  printf ( "Elapsed consumer time is %d sec and %d usec\n", i, j );

  printf ( "\n\n ALL CONSUMERS FINISHED, KILLING  PROCESS\n\n" );
  exit ( 0 );
}



/*********************************************/
/* INITIAL PART OF PRODUCER.....             */
/*********************************************/

void *producer ( void *arg )
{
  int i, j, k;
  unsigned short xsub [3];
  struct timeval randtime;

  gettimeofday ( &randtime, ( struct timezone * ) 0 );
  xsub [0] = ( ushort )randtime.tv_usec;
  xsub [1] = ( ushort ) ( randtime.tv_usec >> 16 );
  xsub [2] = ( ushort ) ( pthread_self() );

  while ( 1 ) {
    j = nrand48 ( xsub ) & 3;
    pthread_mutex_lock ( &prod [j] );
    while ( shared_ring.spaces [j] == 0 ) {
      pthread_cond_wait ( &prod_cond [j], &prod [j] );
    }

    shared_ring.serial[j] = shared_ring.serial[j] + 1;
    shared_ring.flavor[j][shared_ring.in_ptr[j]] = shared_ring.serial[j];
    shared_ring.in_ptr[j] = (shared_ring.in_ptr[j] + 1) % NUMSLOTS;
    shared_ring.spaces[j] = shared_ring.spaces[j] - 1;

    /* unlock producer */
    pthread_mutex_unlock (&prod [j]);

    /* Lock consumer */
    pthread_mutex_lock (&cons[j]);

    shared_ring.donuts[j] = shared_ring.donuts[j] + 1;
    pthread_mutex_unlock (&cons[j]);
    pthread_cond_signal(&cons_cond[j]);


  }

  return NULL; 

}

void    *consumer ( void *arg )
{
  int     s, i, j, k, m, id;
  unsigned short xsub [3];
  struct timeval randtime;
  
  


  /* Donut vars */
  int type; 
  int donuts[4][12];

  /* Consumer counters */
  
  int count1=0;
	int count2=0;
	int count3=0;
	int count4=0;
	int jelly[12]={};
	int coconut[12]={};
	int plain[12]={};
	int glazed[12]={};
char *dtype[] = {"jelly" , "coconut", "plain", "glazed"};
  /* Utility vars */
  struct tm* ptm;
  char szTime[40];
  long ms;

  /* Get a 9 letter file name */ 
  char *fName = (char *) malloc(sizeof(char) * 10);
  threadNum++; 
  int xx;
  xx=threadNum;
  sprintf(fName, "c%d", threadNum);
  FILE *fp = fopen(fName, "w");
 //fprintf(fp, "\n............Thread: %d\t\t\n\n", threadNum);//threadNum);
  /* Random numbers */
  id = *(int *) arg;
  gettimeofday ( &randtime, ( struct timezone * ) 0 );
  xsub [0] = ( ushort ) ( randtime.tv_usec );
  xsub [1] = ( ushort ) ( randtime.tv_usec >> 16 );
  xsub [2] = ( ushort ) ( pthread_self() );

  /* Make 200 dozen donuts */ 
  for( i = 0; i < 200; i++ ) {
    /* Reset or init counts to zero */
   
    for( m = 0; m < 12; m++ ) {

      /* Get donut */
      j = nrand48(xsub) &3;
      
      /* Lock consumer for this flavor */
      pthread_mutex_lock(&cons[j]);
      
      /* Wait for more donuts */
      while(shared_ring.donuts[j] == 0) {
	pthread_cond_wait(&cons_cond[j], &cons[j]); 
      }

      /* Get type */
int donut;
     donut= shared_ring.flavor[j][shared_ring.outptr[j]];
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
		


      /* Reset outptr it we need to */
    
	shared_ring.outptr[j] = (shared_ring.outptr[j]+1) % NUMSLOTS;

      /* Decrement count */
      shared_ring.donuts[j] = shared_ring.donuts[j] - 1;

      /* Lock and unlock */
      pthread_mutex_unlock(&cons[j]);
      pthread_mutex_lock(&prod[j]);

      /* Took a space so make sure it is not taken again */
      shared_ring.spaces[j] = shared_ring.spaces[j] + 1; 

      /* Unlock producer since we are all done */
      pthread_mutex_unlock(&prod[j]);

      /* Signal to producer we are complete */
      pthread_cond_signal(&prod_cond[j]);

    } /* End of inner for loop */

    /* Only write to file the first 10 dozen */ 
    if(i < 10){
      /* Print out stuff */
      ptm = localtime(&randtime.tv_sec);

      /* Time formatting */
      strftime(szTime, sizeof(szTime), "%H:%M:%S", ptm);
      int ms = randtime.tv_usec / 1000;

      fprintf(fp, "\nThread: %d\t\t", xx);
      fprintf(fp, "Time: %s.%d\t\t", szTime, ms);
      fprintf(fp, "Dozen number: %d\n", i + 1);

      /* FLAVORS */
      fprintf(fp, "Jelly\t\tCoconut\t\tPlain\t\tGlazed\n");

      
for(s=0;s<12;s++){
			fprintf(fp,"%d\t\t%d\t\t%d\t\t%d\t\t\n",jelly[s],coconut[s],plain[s],glazed[s]);}
    
  

    } /* End of outer for loop */



    /*****************************************************************/
    /* USING A MICRO-SLEEP AFTER EACH DOZEN WILL ALLOW ANOTHER       */
    /* CONSUMER TO START RUNNING, PROVIDING DESIRED RANDOM BEHVIOR   */
    /*****************************************************************/

    usleep(1000); /* sleep 1 ms */
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

  /* All done with file */
  fclose(fp);

  return NULL;

}
/***********************************************************/
/* PTHREAD ASYNCH SIGNAL HANDLER ROUTINE...                */
/***********************************************************/

void    *sig_waiter ( void *arg ) {
  sigset_t sigterm_signal;
  int signo;

  sigemptyset ( &sigterm_signal );
  sigaddset ( &sigterm_signal, SIGTERM );
  sigaddset ( &sigterm_signal, SIGINT );


  if (sigwait ( &sigterm_signal, & signo)  != 0 ) {
    printf ( "\n  sigwait ( ) failed, exiting \n");
    exit(2);
  }

  printf ( "process going down on SIGNAL (number %d)\n\n", signo );
  exit ( 1 );
  return NULL;

}


/**********************************************************/
/* PTHREAD SYNCH SIGNAL HANDLER ROUTINE...                */
/**********************************************************/

void sig_handler ( int sig ){
  pthread_t signaled_thread_id;
  int i, thread_index;

  signaled_thread_id = pthread_self ( );
  for ( i = 0; i < (NUMCONSUMERS + 1 ); i++) {
    if ( signaled_thread_id == thread_id [i] ) {
      thread_index = i;
      break;
    }
  }

  printf ( "\nThread %d took signal # %d, PROCESS HALT\n",
  thread_index, sig );
  exit ( 1 );

}


