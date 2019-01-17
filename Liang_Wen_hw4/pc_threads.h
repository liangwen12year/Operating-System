
/*****************************************************************/
/* INCLUDE FILES, DEFINES AND DECLARATIONS AS SEEN IN CLASS      */
/*        THESE BELONG IN A    .h     FILE                       */
/*****************************************************************/


#include <signal.h>
#include <sys/time.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#define         NUMFLAVORS       	4
#define         NUMSLOTS        	600
#define         NUMCONSUMERS     	50
#define		NUMPRODUCERS		30


struct  donut_ring {
	int     	flavor [NUMFLAVORS] [NUMSLOTS];
	int     	outptr [NUMFLAVORS];
	int		in_ptr [NUMFLAVORS];
	int		serial [NUMFLAVORS];
	int		spaces [NUMFLAVORS];
	int		donuts [NUMFLAVORS];
};

/**********************************************************************/
/* SIGNAL WAITER, PRODUCER AND CONSUMER THREAD FUNCTIONS              */
/**********************************************************************/

	void	*sig_waiter ( void *arg );
	void	*producer   ( void *arg );
	void	*consumer ( void *arg) ;
	void    sig_handler ( int );




/*#ifndef _MAIN_H
#define _MAIN_H

#include <signal.h>
#include <sys/time.h>
#include <pthread.h>

#define NUMFLAVORS       4
#define NUMSLOTS         30500
#define NUMCONSUMERS     50
#define NUMPRODUCERS     30


struct  donut_ring {
  int     flavor[NUMFLAVORS][NUMSLOTS];
  int     outptr[NUMFLAVORS];
  int     in_ptr[NUMFLAVORS];
  int     serial[NUMFLAVORS];
  int     spaces[NUMFLAVORS];
  int     donuts[NUMFLAVORS];
};

/**********************************************************************/
/* SIGNAL WAITER, PRODUCER AND CONSUMER THREAD FUNCTIONS              */
/**********************************************************************/

/*void* sig_waiter ( void *arg );
void* producer   ( void *arg );
void* consumer ( void *arg);
void sig_handler ( int );

#endif*/
