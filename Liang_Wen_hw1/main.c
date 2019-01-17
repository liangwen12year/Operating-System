#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <sys/resource.h>
#include <wait.h>
#include <stdlib.h>

typedef union {
	int exit_status;
	struct {
		unsigned sig_num : 7;
		unsigned core_dmp : 1;
		unsigned exit_num : 8;
	}parts;
}LE_Wait_Status;

int counter = 0, counter_2G = 0;

void sig_handler(int signal) {
	printf("\nCHILD PROG: Awake in handler - You Got Me With Signal Number %d after spinning for more than %d %s loop iterations\n", signal, 
	((counter_2G) ? counter_2G * 2 : counter),((counter_2G) ? "Billion" : "\0"));
	printf("CHILD PROG:Now beginning to exec target program, goodbye\n");
	fflush(stdout);
	execl("./A1", "prof program", NULL); //no if statement required
	perror("if you see this we failed exec system call, goodbye\n");
	exit(4);
}

int main(void)
{
	pid_t	pid, ppid;
	int	ruid, rgid, euid, egid;
	int	priority;
	char read_buf[100];
	sigset_t mask, proc_mask;
	struct sigaction new_act;
	int synch_pipe[2];
	LE_Wait_Status exit_union;

	if (pipe(synch_pipe) == -1) {
		perror("Unfortunately, failed in Parent pipe creation:");
		exit(8);
	}

	printf("\nThis is the Parent process report:\n");
	pid = getpid();
	ppid = getppid();
	ruid = getuid();
	euid = geteuid();
	rgid = getgid();
	egid = getegid();
	if ((priority = getpriority(PRIO_PROCESS, 0)) == -1){
		perror("getpriority failed ");
		exit(3);
	}

	printf("\nPARENT PROG: Process ID is:\t\t%d\n\
PARENT PROG : Process parent ID is : \t%d\n\
PARENT PROG : Real UID is : \t\t%d\n\
PARENT PROG : Real GID is : \t\t%d\n\
PARENT PROG : Effective UID is : \t%d\n\
PARENT PROG : Effective GID is : \t%d\n\
PARENT PROG : Process priority is : \t%d\n\
		", pid, ppid, ruid, rgid, euid, egid, priority);

	printf("\nPARENT PROG:will now spawn the child, read pipe,\n \
signal child, and wait for child die\n");

	switch (pid = fork()) {
	case -1: perror("Parent cannot spawn the child, goodbye:");
		exit(4);
	default: printf("\nPARENT PROG:created Child with %d PID\n", pid);
		break;
	case 0: /******** CHILD'S CODE*********/
		sigemptyset(&mask);
		new_act.sa_mask = mask;
		new_act.sa_handler = sig_handler;
		new_act.sa_flags = 0;

		if (sigaction(SIGTERM, &new_act, NULL) == -1) {
			perror("failed in sigaction:");
			exit(2);
		}

		printf("\nThis is the Child process report:\n");
		pid = getpid();
		ppid = getppid();
		ruid = getuid();
		euid = geteuid();
		rgid = getgid();
		egid = getegid();
		if ((priority = getpriority(PRIO_PROCESS, 0)) == -1) {
			perror("getpriority failed");
			exit(6);
		}

		printf("\nCHILD PROG: Process ID is:\t\t%d\n\
CHILD PROG : Process parent ID is : \t%d\n\
CHILD PROG : Real UID is : \t\t%d\n\
CHILD PROG : Real GID is : \t\t%d\n\
CHILD PROG : Effective UID is : \t%d\n\
CHILD PROG : Effective GID is : \t%d\n\
CHILD PROG : Process priority is : \t%d\n\
		", pid, ppid, ruid, rgid, euid, egid, priority);

		printf("\nCHILD PROG: about to write pipe and go to endless loop\n");
		fflush(stdout);
		if (write(synch_pipe[1], "homework", 8) == -1) {
			perror("write pipe failed");
			exit(2);
		}

		while (1) {  //child cannot escape except for signal
			counter++;
			if (counter < 0) {
				counter = 0;
				counter_2G++;
			}

		}

	} //Switch and child end

	if (read(synch_pipe[0], read_buf, 1) == -1) {
		perror("read pipe failed ");
		exit(1);
	}
	if (kill(pid, SIGTERM) == -1) {
		perror("kill failed");
		exit(6);
	}
	printf("\nPARENT PROG: read pipe and sent SIGTERM, now wait exit\n");
	if ((pid = wait(&exit_union.exit_status)) == -1) {
		perror("Wait failed in Parent, goodbye:");
		exit(8);
	}

	printf("\nPARENT PROG: CHILD %d exited with %s code %d%s, goodbye\n",
		pid, (exit_union.parts.sig_num == 0) ? "exit" : "signal",
		(exit_union.parts.sig_num == 0) ? exit_union.parts.exit_num : exit_union.parts.sig_num,
		(exit_union.parts.core_dmp) ? " and a coredump" : "\0");
	return 0;
}
			
			
	
	












	

	
