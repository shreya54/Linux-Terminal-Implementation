#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#define bufsize 1024
char *home_directory;
//char *current_directory;

//char *current_directory;

void change_directory_display(char * default_dir, char * current_directory,char* hostname,char* username)
{
    int count_0 =0, count_1=1; //replaced by i and j respectively
    if(strlen(current_directory) > strlen(home_directory))
    {
        while(home_directory[count_0]!='\0')
            count_0 += 1;
        while(current_directory[count_0]!='\0')
            default_dir[count_1++] = current_directory[count_0++];

    }
    else if(strlen(current_directory)<strlen(home_directory))
        strcpy(default_dir, current_directory);

    //printf("printing prompt\n");
    printf("%s@%s:%s>",username,hostname,default_dir);

}

void prompt()
{
    char *username,*hostname;
    username=(char *)malloc(bufsize*sizeof(char));
    hostname=(char *)malloc(bufsize*sizeof(char));
    username=getenv("USER");
    gethostname(hostname,bufsize);
    //printf("%s@%s:~>", username,hostname);

    char *default_dir;
    default_dir=(char*)malloc(bufsize*sizeof(char));
    int i,j;
    for(i=0; i<2; i++)
    {
        if(i==0)
            default_dir[0] = '~';
        else if(i==1)
            default_dir[1] = '\0';
    }
    //printf("default dir initial: %s",default_dir);
    char *current_directory;
    
    current_directory = (char*)malloc(bufsize*sizeof(char));
    getcwd(current_directory, bufsize);

    change_directory_display(default_dir, current_directory,hostname,username);
    //printf("default dir after: %s",default_dir);
            
}


