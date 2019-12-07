#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
 
#define bufsize 1024
#define TOKEN_BUFSIZE 64
#define DELIM " \t\r\n\a "
//#include "prompt.c"
//#include "functions.c"
void prompt();
char *home_directory;
int shell_pid;
int run=0;
int flag;

int execute(char ** arguments);
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#define bufsize 1024

typedef struct jobs {
  pid_t pid;
  int stat;
  char name[50];

}jobs;
jobs jobs_buf[bufsize];
int job_no;

typedef struct fg_jobs {
  pid_t pid;
  int stat;
  char name[50];

}fg_jobs;
fg_jobs fg_jobs_buf[bufsize];
int fg_job_no;

void sigchld_handler(int signum);
//The waitpid() system call suspends execution of the calling process until a child specified by pid argument has changed state.
//-1 means meaning wait for any child process.//The parent can use the system call wait() or waitpid() // In the case of a deceased child, as soon as a status has been reported, the zombie vanishes. 
int ctrl_c_flag;
int piping(char **pipes);//pipes[0]=ls -l
//pipes[1] grep


char *reading_line(void)
{
  //int bufs = BUFSIZE;
  char *line = NULL;
  ssize_t bufs = 0; // have getline allocate a buffer for us
  getline(&line, &bufs, stdin);
  return line;

}
char* read_cmd()
{
  int count=0;
  int pos=0;
  int c;
  char *buf;
  buf=(char*)malloc(sizeof(char)*bufsize);
  if(!buf)
  {
    exit(EXIT_FAILURE);
  }
  while(1)
  {
    c=getchar();
    /*if(!count && c==EOF)
    {
      kill(shellpid, SIGKILL);
      //char *chh;
      //chh=(char*)malloc(sizeof(char)*2);
      //strcpy(chh, "#");
      //return chh;
    }*/
    if(c==EOF || c=='\n')
    {
      buf[pos]='\0';
      return buf;
    }
    else
      buf[pos]=c;
    pos++;
    count++;
  }
} 

char **spliting_pipe(char *line, char *delim)//NEW 
{
  int pos=0;
  char *buf;
  buf=(char*)malloc(bufsize*sizeof(char));
  char **tokens;
  tokens=malloc(bufsize*(sizeof(char*)));

  if(!tokens)
  {
    exit(EXIT_FAILURE);
  }
  buf=strtok(line, delim);

  while (buf!=NULL)
  {
    tokens[pos++]=buf;
    buf=strtok(NULL, delim);
  }
  tokens[pos]=NULL;
  return tokens;
}

char **spliting_line(char *line)
{
    int token_bufsize= TOKEN_BUFSIZE;
    int index =0;
    //allocate memory to arguments
    char **arguments = malloc(token_bufsize * sizeof(char*));//storing tokens in arguments array
      char *token;
      if (!arguments)
      {
    fprintf(stderr, "allocation error\n");
    exit(1);//exit failure
    }
    token = strtok(line, DELIM);
    
    //strtok//This function returns a pointer to the last token found in the string. A null pointer is returned if there are no tokens left to retrieve.
    //and place \0 bytes at the end of each token.
    while (token != NULL)
    {
    //storing tokens in arguments
    arguments[index] = token;
    index++;
    if (index >= token_bufsize)//exceeded memory size
    {
      //Reallocation(token_bufsize,arguments)
      token_bufsize += TOKEN_BUFSIZE;
      arguments = realloc(arguments, token_bufsize * sizeof(char*));
      if (!arguments) {
        fprintf(stderr, "allocation error\n");
        exit(1);
      }
      
      }
      token = strtok(NULL, DELIM);
      }
      //null terminate the list of tokens
      arguments[index] = NULL;
      return arguments;
}
void ctrl_c(int signum)//working for background processes also //but should only work for foreground processes  
{
  int ret_kill;
  //printf("pid %d\n",getpid());
  if(getpid()!=shell_pid)
  {
    //printf("inside if\n");
    ret_kill=kill(getpid(), SIGKILL);
    //printf("ret_kill %d\n",ret_kill);

  }
  //signal(SIGINT, ctrl_c);
  //return ctrl_c_flag;

}
void ctrl_z(int signum)//It should change the status of currently running job to stop, and push it in
//background process.
{
  //int ret_kill;
  //printf("hello ctrlc\n");
  signal(SIGTSTP, ctrl_z);
  int ret_z;

  //printf("fg_job_no before %d\n",fg_job_no);
  //printf("bg_job_no before %d\n",job_no);
  if(fg_jobs_buf[fg_job_no-1].stat==1)//pushing it to background process
  {
  //printf("fg_job_no %d status is 1 inside if\n",fg_job_no-1);
  //job_no++;
  jobs_buf[job_no].stat=1;
  strcpy(jobs_buf[job_no].name,fg_jobs_buf[fg_job_no-1].name);
  jobs_buf[job_no].pid=fg_jobs_buf[fg_job_no-1].pid;
  //printf("fg job-1 pid %d\n",fg_jobs_buf[fg_job_no-1].pid);
  //printf("fg job pid %d\n",fg_jobs_buf[fg_job_no].pid);
  ret_z=kill(fg_jobs_buf[fg_job_no-1].pid,SIGSTOP);
  if(errno==-1)
  {
  printf("%d\n",errno);

  perror("Error");
  }
  fg_jobs_buf[fg_job_no-1].stat=0;
  printf("\n");
  printf("stop and push to background(Ctrl-Z)\n");
  //job_no++;//No need of incrementing job no . already incremented 
  }
  //signal(SIGTSTP, ctrl_z);
  /*printf("ret_z %d\n",ret_z);
  if(ret_z==-1)
  {
  fprintf(stderr, "%s\n", strerror( errno ));
  }*/
  //printf("fg_job_no after %d\n",fg_job_no);
  //printf("bg_job_no after%d\n",job_no);

  
  
  //return ctrl_c_flag;

}
char **split_cmd(char *line, char *delim)
{
  int pos=0;
  char *buf;
  buf=(char*)malloc(bufsize*sizeof(char));
  char **tokens;
  tokens=malloc(bufsize*(sizeof(char*)));

  if(!tokens)
  {
    exit(EXIT_FAILURE);
  }
  buf=strtok(line, delim);

  while (buf!=NULL)
  {
    tokens[pos++]=buf;
    buf=strtok(NULL, delim);
  }
  tokens[pos]=NULL;
  return tokens;
}
int main(int argc, char* argv[])
{
            char * line;
            char ** arguments;
            char ** commands;
            char ** pipes;
            int i;
            flag=1;
            home_directory=(char *)malloc(bufsize*sizeof(char));
            getcwd(home_directory, bufsize);
            int count=0;
            int p;
            for(p=0; p<bufsize; p++)
            {
              jobs_buf[p].stat=0;
              jobs_buf[p].stat=0;

            }
            //signal(SIGCHLD, sigchld_handler);//Whenever something interesting happens to the child (it exits, crashes, traps, stops, continues), and in particular when it dies, the parent is sent a SIGCHLD signal.
            signal(SIGINT, ctrl_c);
            signal(SIGTSTP, ctrl_z);

            //act.sa_handler = ctrl_c(signum);
            //sigaction(SIGINT, &act, NULL); 
            //printf("%d\n",ctrl_c_flag);
            
            while(flag==1)
            {
            //printf("print prompt after signal");
            prompt();
            if (count==0)
            {
              shell_pid=getpid();
              //printf("%d",shell_pid);
            }
            //line=(char*)malloc(sizeof(char)*BUFSIZE);
            //line = reading_line();
            line=read_cmd();
            //printf("%s",line);
            
            
            commands=split_cmd(line, ";");
            //printf("commands[0] %s\n",commands[0]);          

            //printf("commands[1] %s\n",commands[1]);          

            //printf("commands[2] %s\n",commands[2]);          
            for (i=0; commands[i]!=NULL; i++)
            {

            //arguments=spliting_line(line);
            pipes=split_cmd(line,"|");
            //printf("pipes[0] %s\n",pipes[0]);          

            //printf("pipes[1] %s\n",pipes[1]);          

            //printf("pipes[2] %s\n",pipes[2]); 

            
            if(pipes[1]!=NULL)
            {
              //printf("Piping\n\n");
              flag=piping(pipes);
            }
            else
            {
            arguments=spliting_line(pipes[i]);
            flag=execute(arguments);
            //printf("%dth args(pipe) %s arg[0]\n",i,arguments[0]);
            //printf("%dth args(pipe) %s arg[1]\n",i,arguments[1]);

            }
            }
            //int i=0;
            /*while(arguments[i]!=NULL)
            {    
            printf("%s ",arguments[i]);
            i=i+1;
            }*/           
            
            
            signal(SIGCHLD, sigchld_handler);//Whenever something interesting happens to the child (it exits, crashes, traps, stops, continues), and in particular when it dies, the parent is sent a SIGCHLD signal.
            //printf("flag for nightswatch %d\n",flag);
           // signal(SIGTSTP, ctrl_z);
            count=count+1;
           // printf("flag %d",flag); 
        //break if flag equals 1 
            
            }
}
//Problems : kjob only implemented for background jobs 
//Whenever we kill a process , jobs status shows stopped not killed
//printing the prompt again after ctrl c and ctrl z
