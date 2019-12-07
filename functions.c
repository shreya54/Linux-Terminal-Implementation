#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#define bufsize 1024 //BUFSIZE changed to bufsize
//#define BUFSIZE 1024
#define TOKEN_BUFSIZE 64
#define DELIM " \t\r\n\a"
#include <sys/types.h>
#include <dirent.h>
#include <curses.h>
#include <errno.h>
#include <signal.h>
#include <sys/file.h>
#include <syscall.h>
//#include "start.c"
//#include "key.c"
//
char *home_directory;
int shell_pid;
char **spliting_line(char *line);
int execute(char ** arguments);
char **split_cmd(char *line, char *delim);
int start(char ** arguments);
int ls(char ** arguments);
int ls_l(char ** arguments);
int ls_a(char ** arguments);
int ls_la(char ** arguments);
int ret_setenv;
int ret_unsetenv;
int in,out;
typedef struct jobs {
	pid_t pid;
	int stat;
	char name[50];

}jobs;

jobs jobs_buf[bufsize];
int job_no;



int pwd(char **arguments)
{
    char *cur_dir = (char *)malloc(bufsize*sizeof(char));
    getcwd(cur_dir, bufsize);
    printf("%s\n", cur_dir );
    return 1;
}

int ECHO(char **arguments)
{
    int i=1;
    while(arguments[i]!=NULL)
    {
    	 printf("%s ", arguments[i] );
        i++;
    }
    printf("\n");
    return 1;
}


int cd(char **arguments)
{
    //printf("Calling cd\n");
    int ret_val;

    if(arguments[1]==NULL)
    {
            chdir(home_directory);
    }
    else if(arguments[1]=="~")
    {    
            chdir(home_directory);
    }
    else
    {
        ret_val = chdir(arguments[1]);
        if(ret_val!=0)
            fprintf(stderr, "Directory doesn't exist.\n");
    }
    
    return 1;
}

int EXIT()
{
    printf("Thankyou\n");
    return 0;
}
int pinfo(int shell_pid,char ** arguments)
{
	char **values;

	char stat_path[1024];
	char exec_path[1024];
	int hola=0;
	size_t len=0;
	//DIR* proc;
	FILE *fptr,*fptr1;
	printf("pid--  %d\n",shell_pid);
	sprintf(stat_path, "/proc/%d/stat", shell_pid);
	//proc=opendir("/proc");
	//printf("stat_path %s\n",stat_path);
	fptr=fopen(stat_path, "r");
	if(fptr==NULL)
	{
		perror("Could not open (/proc/pid/stat)");
    	return 1;
	}
	else
	{
		int i;
		char *line = NULL;
		getline(&line,&len,fptr);
		values=spliting_line(line);
		//values=strtok(line," ");
		i=0;

		/*while(values[i]!=NULL)
            {*/    
           	//printf("%s ",values[2]);
            //i=i+1;
            //}
		printf("Process Status --  %s\n",values[2]);
		printf("Virtul Memory Size -- %s\n",values[22]);
		if (atoi(values[22])==0)
		{
			printf("Execuatble Path:Process is currently not executing\n");	
			hola=1;
		}


	}
	sprintf(exec_path, "/proc/%d/cmdline", shell_pid);
	fptr1=fopen(exec_path, "r");
	if(fptr1==NULL)
	{
		perror("Could not open (/proc/pid/cmdline)");
    	return 1;
	}
	else
	{	
		char *line1 = NULL;
		//char **values1;
		getline(&line1,&len,fptr1);
		//printf ("%s",line1);
		//values1=spliting_line(line1);
		
		if(hola!=1)
		{
		printf("Executable path: ~%s\n",line1);
		}
	}




	return 1;
}
int dirty()
{
	FILE *fptr;
	char *line1 = NULL;
	size_t len=0;
	char ** dirty;

	fptr=fopen("/proc/meminfo","r");
	getline(&line1,&len,fptr);
	getline(&line1,&len,fptr);
	getline(&line1,&len,fptr);
	getline(&line1,&len,fptr);
	getline(&line1,&len,fptr);
	getline(&line1,&len,fptr);
	getline(&line1,&len,fptr);
	getline(&line1,&len,fptr);
	getline(&line1,&len,fptr);
	getline(&line1,&len,fptr);
	getline(&line1,&len,fptr);
	getline(&line1,&len,fptr);
	getline(&line1,&len,fptr);
	getline(&line1,&len,fptr);
	getline(&line1,&len,fptr);
	getline(&line1,&len,fptr);
	getline(&line1,&len,fptr);
	//printf("%s\n",line1);
	dirty=spliting_line(line1);
	int i=0;
    //fflush(stdin);  
    printf("%s%s\n",dirty[1],dirty[2]);
    //i=i+1;
    //} 
	
	return 1;

	//printf("%s\n",line1);



}
int overkill(char **arguments)
{
	int i;
	for(i=0; i<job_no; i++)
	{
		if(jobs_buf[i].stat==1)
		{
			kill(jobs_buf[i].pid, 9);
			if(errno!=0)
			{
			printf("%d\n",errno);

			perror("Error");
			}
		}
	}
	return 1;
}
int fg(char **arguments)//when processes are in stopped state (Ctrl-z) we can move a process from backgr to foregr or foregr to back
{
	//brings a running or a stopped background job with given job number to foreground.
  int ret_fg;

  if(arguments[1]==NULL)
  {
    printf("Usage: fg [process no.]\n");
  }
  int proc_no =atoi(arguments[1]);
  //printf("job_no=%d\n",job_no);
  //printf("jobs_buf[proc_no].name=%s\n",jobs_buf[proc_no].name);
  if(proc_no>job_no)//running or stopped background to foreground
  {
  	//stat should be 1 if bg process running 
    printf("ERROR: No such background process(running) number exists.\n");
    return 1;
  }

  //printf("pid of the proc sent to backgr %d\n",jobs_buf[proc_no].pid);
  ret_fg=kill(jobs_buf[proc_no].pid,18);//SIGCONT//When SIGSTOP or SIGTSTP is sent to a process, the usual behaviour is to pause that process in its current state.
  // The process will only resume execution if it is sent the SIGCONT signal.
  if(ret_fg==-1)
  {
  fprintf(stderr, "%s\n", strerror( errno )); 
  }
  //printf("ret_fg=%d\n",ret_fg);
  jobs_buf[proc_no-1].stat=0;
  return 1;
}
//job                    # starts the job
//$ <CTRL-Z>               # stops the job
//$ bg                     # puts the job in background
//$ nohup <PID>            # unattaches the backgrounded job from the terminal
//Control-Z suspends the most recent foreground process (the last process to interact with the tty) 
//(unless that process takes steps to ignore suspension, like shells normally do).
// This will generally bring you back to your shell, from which you can generally enter the command bg to move the just-suspended process to the background (letting it continue to run) or fg to bring it back to the foreground.
//Ctrl – C : this sends the SIGINT signal to the current process, killing it. This is often used when a process freezes.
//Ctrl – Z : this sends the SIGSTOP signal to the current process, suspending it. This is mostly used for sending a process in the run in the background.
int bg(char **arguments)//when processes are in stopped state (Ctrl-z) we can move a process from backgr to foregr or foregr to back
{
	//changes stopped background job to running background job 
  int ret_bg;

  if(arguments[1]==NULL)
  {
    printf("Usage: bg [process no.]\n");
  }
  //we will have to check processes running in foreground 
  int proc_no =atoi(arguments[1]);
  printf("job_no=%d\n",job_no);
  //printf("jobs_buf[proc_no].name=%s\n",jobs_buf[proc_no].name);
  if(proc_no>job_no || jobs_buf[proc_no-1].stat!=0)//********************THIS to be added in prakrati's file/////
  {

  	//stat should be 1 if bg process running 
    printf("ERROR: No such background process(stopped) number exists.\n");
    return 1;
  }


  ret_bg=kill(jobs_buf[proc_no].pid,18);//SIGCONT//When SIGSTOP or SIGTSTP is sent to a process, the usual behaviour is to pause that process in its current state.
  // The process will only resume execution if it is sent the SIGCONT signal.
  /*if(ret_bg==-1)
  {
  fprintf(stderr, "%s\n", strerror( errno )); 
  }*/
  //printf("ret_bg=%d\n",ret_bg);
  if(jobs_buf[proc_no-1].stat==0)
  {
  jobs_buf[proc_no-1].stat=1;//foreground to background 
  }
  return 1;
}
int kbhit(void)
{
    int ch = getch();

    if (ch != ERR) {
        ungetch(ch);
        return 1;
    } else {
        return 0;
    }
}
int nightswatch(char **arguments)
{    
    int i=5;
    int c;
    int flag_watch = 0;
    int secs = atoi(arguments[2]);
    int q;
    FILE *fptr;
    int flag1=1;

    initscr();

    //cbreak();
    //noecho();
    nodelay(stdscr, TRUE);

    //scrollok(stdscr, TRUE);
    while (1) {
    	sleep(secs);
        flag_watch = dirty();


        if (kbhit()) {
        	
        	if(getch()==113)
        	{
            //printf("Key q pressed!");
            break;
        	}

            refresh();
        } 
    }
    endwin();


    
    flag_watch=1;
    //flag1 =0;
    //printf("flagwatch before returning %d\n",flag_watch);
    return flag_watch;
}
int listing_jobs()
{
	int i;
	int counter=0;
	for(i=0; i<job_no; i++)
	{
		//printf("job_no %d\n",job_no);
		//printf("jobs_buf[0].stat %d\n",jobs_buf[0].stat);
		if(jobs_buf[i].stat==1)
		{

			counter++;
			printf("[%d] Running %s [%d]\n", counter, jobs_buf[i].name, jobs_buf[i].pid);
		}
		else
		{
			counter++;

			printf("[%d] Stopped %s [%d]\n", counter, jobs_buf[i].name, jobs_buf[i].pid);

		}
	}
	return 1;
}
int kjob(char **arguments)//takes the job id of a running job and sends a signal value to
//that process//made it only for background job 
{
	int ret_kill;
	//printf("arguments[1]=%s\n",arguments[1]);
	if (arguments[1]==NULL || arguments[2]==NULL)
	{
	perror("Usage: kjob [process no.] [signal no.]");
	return 1;
	}

	int job_id=atoi(arguments[1]);//process no.
	int sig_no=atoi(arguments[2]);

	if(job_id==0)
	{
		printf("Error : Job ID cannot be zero!\n");
		return 1;
	}
	if(jobs_buf[job_id-1].stat!=1)
	{
		printf("Needs job id for a running job\n");
		return 1;
	}
	//CONDITION TO check if the job exists or not //not included yet
	//kill only the job which is running 
	/*int i;
	
	if(count!=job_id)
	{
		printf("Error : Such a job id does not exist\n");
		return 1;
	}*/
	//printf("job_id=%d\n",job_id);
	//printf("jobs_buf[job_id-1].pid = %d\n",jobs_buf[job_id-1].pid);
	ret_kill=kill(jobs_buf[job_id-1].pid, sig_no);
	if(errno!=0)
	{
	//printf("%d\n",errno);

	perror("Error");
	}
	//on success 0 returned
	//fprintf(stderr, "%s\n", strerror( errno)); 
	//printf("ret_kill =%d\n",ret_kill); 
	return 1;
}
char** redirect(char **arguments)
{
	char *to_file, *from_file;
	int total, i, dupp, closee, to_fd, from_fd;
	for(i=0; arguments[i]!=NULL; i++);
	total=i;
	for(i=0; i<total; i++)
	{
		if(!strcmp(arguments[i], ">"))
		{
			if(i==total-1)
			{
				printf("bash: syntax error near unexpected token `newline'\n");
				return arguments;
			}
			else
			{
				to_file=(char*)malloc(sizeof(char)*bufsize);
				strcpy(to_file, arguments[i+1]);

				creat(to_file, 0777);

				to_fd = open(to_file, O_WRONLY | O_CREAT, 0777);
				if(to_fd==-1)
				{
					printf("Error in opening file.\n");
					return arguments;
				}
				dupp=dup2(to_fd, 1);//// redirect output to the file
				if(dupp==-1)
				{
					perror("Error in executing dup2!  :\n");
				}
				closee=close(to_fd);
				if(closee==-1)
					printf("Not closed outfile properly!\n");
			}
			arguments[i]=NULL;
		}
		else if(!strcmp(arguments[i], "<"))
		{
			if(i==total-1)
			{
				printf("bash: syntax error near unexpected token `newline'\n");
				return arguments;
			}
			else
			{
				from_file=(char*)malloc(sizeof(char)*bufsize);
				strcpy(from_file, arguments[i+1]);

				from_fd = open(from_file, O_RDONLY);
				if(from_fd==-1)
				{
					printf("Error in opening file.\n");
					return arguments;
				}
				dupp=dup2(from_fd, 0); 
				if(dupp==-1)
					printf("Error in executing dup2!\n");
				closee=close(from_fd);
				if(closee==-1)
					printf("Not closed outfile properly!\n");
			}
			arguments[i]=NULL;
		}
		else if(!strcmp(arguments[i], ">>"))
		{
			if(i==total-1)
			{
				printf("bash: syntax error near unexpected token `newline'\n");
				return arguments;
			}
			else
			{
				to_file=(char*)malloc(sizeof(char)*bufsize);
				strcpy(to_file, arguments[i+1]);

				to_fd = open(to_file, O_RDONLY | O_WRONLY | O_APPEND | O_CREAT, 0777);
				if(to_fd==-1)
				{
					printf("Error in opening file.\n");
					return arguments;
				}
				dupp=dup2(to_fd, 1); 
				if(dupp==-1)
					printf("Error in executing dup2!\n");
				closee=close(to_fd);
				if(closee==-1)
					printf("Not closed outfile properly!\n");
			}
			arguments[i]=NULL;
		}
	}
	return arguments;
}


/*int piping(char **pipes)
{
	pid_t pid;
	char **args;
	char **arguments;
	int in=0, i, total=0, fd[2], err, status=1;
	for(i=0; pipes[i]!=NULL; i++);
	total=i;
	printf("total pipes %d\n",total);


	for(i=0; i<total; i++)
	{
		if(pipe(fd)==-1)
			printf("Error in piping!\n");
		args = split_cmd(pipes[i]," \t");
		printf("%dth args[0] %s\n",i,args[0]);          

        printf("%dth args[1] %s\n",i,args[1]);          

        printf("%dth args[2] %s\n",i,args[2]);   
		/*while(args[i]!=NULL)
            {    
            printf("%dth args %s \n",i,args[i]);
            i=i+1;
            } 
		pid=fork();
		printf("pid = %d \n",pid);
		if(pid==-1)
		{
			printf("Error in forking!\n");
			exit(EXIT_FAILURE);
		}
		else if (pid==0)
		{
			printf("enter pid=0\n");
			err=dup2(in, 0);
			printf("err open = %d\n",err);
			if(err==-1)
				printf("Error while doing dup2!\n");
			err=close(fd[0]);
			if(err==-1)
				printf("Error while closing\n");
			printf("err close = %d\n",err);
			if(i!=(total-1))
			{
				printf("enter conditon\n");
				err=dup2(fd[1], 1);
				if(err==-1)
					printf("Error in dup2\n");
			}
			while(args[i]!=NULL)
            {    
            printf("args %s \n",args[i]);
            i=i+1;
            }  
			//printf("%s ",args);
			status=execute(args);
			printf("%d ",status);
			exit(0);
		}
		else
		{
			printf("wait\n");
			wait(&pid);
			err=close(fd[1]);
			in=fd[0];
		}
	}
	return status;
}*/
int execute(char ** arguments)
{
	int status,i;
	char ** hello;
	in=dup(0);
	out=dup(1);
	arguments=redirect(arguments);
	while(arguments[i]!=NULL)
		i++;

	i--;
	//printf("final i = %d\n",i);
	if(strcmp(arguments[0],"cd")==0)
	{
			status=cd(arguments);	//cd function
	}
	else if(strcmp(arguments[0],"pwd")==0)
	{
			status=pwd(arguments);
	}
	else if(strcmp(arguments[0],"echo")==0)
	{
			status=ECHO(arguments);
	}
	else if(strcmp(arguments[0],"quit")==0)
	{
			status=EXIT();
	}
	else if(strcmp(arguments[0],"pinfo")==0 && arguments[1]==NULL)
	{
			status=pinfo(shell_pid,arguments);
			//status =1;
			//printf("%d",shell_pid);
	}
	else if(strcmp(arguments[0],"pinfo")==0 && arguments[1]!=NULL)
	{
			//printf("hey");
			status=pinfo(atoi(arguments[1]),arguments);
			/*int i=0;
			while(hello[i]!=NULL)
			{
				printf("%s hello",hello[i]);
				i=i+1;
			}
			status  = 1;*/
	}
	else if(strcmp(arguments[0],"nightswatch")==0)
	{
		status=nightswatch(arguments);
		//printf("status %d\n",status);
	}
	else if(strcmp(arguments[0],"ls")==0 && i==0)//1 argument
	{
		status=ls(arguments);

	}
	else if(strcmp(arguments[0],"ls")==0 && i==1)//2 arguments
	{
		if(strcmp(arguments[1],"&")==0)
		{
		status=start(arguments);
		}
		else if(strcmp(arguments[1],"-l")==0)
		{
		status=ls_l(arguments);
		}
		else if(strcmp(arguments[1],"-a")==0)
		{
		status=ls_a(arguments);
		}
		else if(strcmp(arguments[1],"-la")==0||strcmp(arguments[1],"-al")==0)
		{
		status=ls_la(arguments);
		}

	}
	else if(strcmp(arguments[0],"setenv")==0 && i==2)//2arguments
	{
		//printf("hey");
		//printenv command to list all the env variables 
		ret_setenv=setenv(arguments[1],arguments[2],1);//overwrite is 1//if name exists then overwrite 
		if(ret_setenv==-1)
		{
		fprintf(stderr, "%s\n", strerror( errno ));
		}
		//printf("%d\n",ret_setenv);//on success returns 0
		status=1;
	}
	else if(strcmp(arguments[0],"setenv")==0 && i==1)//1 argument
	{
		ret_setenv=setenv(arguments[1],"",1);//set it to empty string 
		if(ret_setenv==-1)
		{
		fprintf(stderr, "%s\n", strerror( errno ));
		}
		//printf("%d\n",ret_setenv);
		status=1;
	}
	else if(strcmp(arguments[0],"setenv")==0 && i==0 || strcmp(arguments[0],"setenv")==0 && i>2 )//0 argument
	{
		printf("Error: More than zero and not more than two command line arguments needed\n");
		status=1;
	}
	else if(strcmp(arguments[0],"unsetenv")==0 && i==1)//1 argument
	{
		ret_unsetenv=unsetenv(arguments[1]);
		if(ret_unsetenv==-1)
		{
		fprintf(stderr, "%s\n", strerror( errno ));
		}
		//printf("%d\n",ret_unsetenv);
		status=1;

	}
	else if(strcmp(arguments[0],"unsetenv")==0 && i==0)//1 argument
	{
		printf("Error: Needs atleast one argument\n");
		status=1;

	}
	else if(strcmp(arguments[0],"jobs")==0)
	{
		status=listing_jobs();
	}
	else if(strcmp(arguments[0],"overkill")==0)
	{
		status=overkill(arguments);
	}
	else if(strcmp(arguments[0],"kjob")==0)
	{
		status=kjob(arguments);
	}
	else if(strcmp(arguments[0],"fg")==0)
	{
		status=fg(arguments);
	}
	else if(strcmp(arguments[0],"bg")==0)
	{
		status=bg(arguments);
	}
	

	//fflush(stdin);
	else
	{
		status=start(arguments);
	}
	dup2(in, 0);
	dup2(out, 1);
	/*else
	{
		printf("No such command\n");
		status=1;
	}*/
	return status;

}
int piping(char **pipes)
{
	pid_t pid;
	char **args;
	char **arguments;
	int in=0, i, total=0, fd[2], err, status=1;
	for(i=0; pipes[i]!=NULL; i++);
	total=i;
	//printf("total pipes %d\n",total);


	for(i=0; i<total; i++)
	{
		if(pipe(fd)==-1)
			printf("Error in piping!\n");
		args = split_cmd(pipes[i]," \t");
		
		pid=fork();
		//printf("pid = %d \n",pid);
		if(pid==-1)
		{
			printf("Error in forking!\n");
			exit(EXIT_FAILURE);
		}
		else if (pid==0)
		{
		//	printf("enter pid=0\n");
			err=dup2(in, 0);
		//	printf("err open = %d\n",err);
			if(err==-1)
				printf("Error while doing dup2!\n");
			err=close(fd[0]);
			if(err==-1)
				printf("Error while closing\n");
		//	printf("err close = %d\n",err);
			if(i!=(total-1))
			{
		//		printf("enter conditon\n");
				err=dup2(fd[1], 1);
				if(err==-1)
					printf("Error in dup2\n");
			}
			/*while(args[i]!=NULL)
            {    
            printf("args %s \n",args[i]);
            i=i+1;
            }*/  
			//printf("%s ",args);
			status=start(args);
			//printf("%d ",status);
			exit(0);
		}
		else
		{
			//printf("wait\n");
			wait(&pid);
			err=close(fd[1]);
			in=fd[0];
		}
	}
	return status;
}