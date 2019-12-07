#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#define bufsize 1024

typedef struct jobs {
	pid_t pid;
	int stat;
	char name[50];

}jobs;
jobs jobs_buf[bufsize];
int job_no;
int flag;

void prompt();
//The waitpid() system call suspends execution of the calling process until a child specified by pid argument has changed state.
//-1 means meaning wait for any child process.//The parent can use the system call wait() or waitpid() // In the case of a deceased child, as soon as a status has been reported, the zombie vanishes. 
void sigchld_handler(int signum)
{
	pid_t wpid;
  //int procstatus;
	int status;
	/////////////////ONLY FOR BG JOBS //////////////////
	//printf("jobs_buf[%d].stat=%d\n",job_no,jobs_buf[job_no].stat);
	wpid=waitpid(-1,&status,WNOHANG);//return immediately if no child has exited. 
	if(wpid>0	&& (WIFEXITED(status)==1))//returns true if the child terminated normally, that is, by calling exit(3) or _exit(2), or by returning from main(). 
	{
			//prompt();
			

			printf("Process with pid %d exited normally\n",wpid);
			jobs_buf[job_no-1].stat=0;
			//job_no.incremented in start file . so decreased by 1 to get the correct index
			flag=1;

	}
	if(wpid >0 && WIFSIGNALED(status)==1)
		{
			//prompt();

			printf("Process with pid %d exited due to a user-defined signal\n",wpid);
			jobs_buf[job_no-1].stat=0;
			flag=1;

		}

}
