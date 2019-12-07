#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#define BUFSIZE 1024

char** redirect(char **arguments);
char *home_dir;

//*** Background Jobs ***///
typedef struct jobs {
	pid_t pid;
	
	int stat;
	char name[50];
}jobs;

jobs jobs_buf[BUFSIZE];
int job_no;

typedef struct fg_jobs {
	pid_t pid;
	
	int stat;
	char name[50];
}fg_jobs;

fg_jobs fg_jobs_buf[BUFSIZE];
int fg_job_no=0;


int start(char **arguments)
{
	pid_t pid, pid2;
	int status;

	//
	arguments=redirect(arguments);
	int i=0, bg_flag=0;

	while(arguments[i]!=NULL)
		i++;

	i--;

	int j=0;

	while(arguments[i][j]!='\0')
		j++;

	//printf("%c\n", arguments[i][j-1]);

	if(!strcmp(arguments[i], "&"))
	{
		bg_flag=1;
		arguments[i]=NULL;
	}
	/*if(bg_flag==0)
	{
	printf("bg not running\n");
	printf("arguments[0] %s\n",arguments[0]);
	printf("fg_job_no %d\n",fg_job_no);
	strcpy(fg_jobs_buf[fg_job_no].name, arguments[0]);
	fg_jobs_buf[fg_job_no].stat=1;
			
	//fg_jobs_buf[fg_job_no].pid1=pid1;
	printf("fg_name %s\n",fg_jobs_buf[fg_job_no].name);
	printf("fg_stat %d\n",fg_jobs_buf[fg_job_no].stat);
	fg_job_no++;

	}*/
	/*else if(arguments[i][j-1]=='&')
	{
		bg_flag=1;
		arguments[i][j-1]='\0';
	}*/
	//
	//printf("bg_flag = %d\n",bg_flag);

	pid=fork();
	if(pid<0)
	{
		fprintf(stderr, "Error forking.");
	}
	else if(pid==0)//child running 
	{
		if(bg_flag==1)
		{
			printf("PROCESS ID=%d\n", getpid());
			//printf("job_no.=%d\n",job_no);
			jobs_buf[job_no].stat=1;
		}
		if(execvp(arguments[0], arguments)==-1)
			exit(EXIT_FAILURE);
		else
			exit(0);
	}
	
	else//parent running
	{
		if(bg_flag!=1)
		{
			//storing foreground jobs
			//printf("bg not running\n");
			//printf("arguments[0] %s\n",arguments[0]);
			//printf("fg_job_no %d\n",fg_job_no);
			strcpy(fg_jobs_buf[fg_job_no].name, arguments[0]);
			fg_jobs_buf[fg_job_no].stat=1;
			fg_jobs_buf[fg_job_no].pid=pid;
			//printf("fg_name %s\n",fg_jobs_buf[fg_job_no].name);
			//printf("fg_stat %d\n",fg_jobs_buf[fg_job_no].stat);
			//printf("fg_pid %d\n",fg_jobs_buf[fg_job_no].pid);

			fg_job_no++;

			do
			{
				waitpid(pid, &status, WUNTRACED);
			}while(!WIFEXITED(status) && !WIFSIGNALED(status));
		}
		else
		{
			//storing only background jobs
			//printf("bg running\n");
			strcpy(jobs_buf[job_no].name, arguments[0]);
			jobs_buf[job_no].stat=1;
			//printf("%d\n",jobs_buf[job_no].pid);
			//printf("%s\n",jobs_buf[job_no].name);
			
			jobs_buf[job_no].pid=pid;
			job_no++;
			//when background jobs stops change status to 0
		}
	}
	return 1;
}

