#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>    
#include <pwd.h>  
#include <grp.h>  
#include <time.h>  
#include <locale.h>  
#include <langinfo.h>  
#include <stdint.h>  
#include <fcntl.h>
#include <stdio.h>

char pathname[MAXPATHLEN];
extern  int alphasort(); 
long getcwd(char *buf,unsigned long size); 
static char perms_buff[30];
//int scandir(const char *dir, struct dirent ***namelist, int (*selector) (struct dirent *), int (*cmp) (const struct dirent **, const struct dirent **));
//char *current_directory;
const char *get_perms(mode_t mode)                      
{  
  char ftype = '?';  
  
        if (S_ISREG(mode)) ftype = '-';                  /*checking the flags type */
        if (S_ISLNK(mode)) ftype = 'l';  
        if (S_ISDIR(mode)) ftype = 'd';  
        if (S_ISBLK(mode)) ftype = 'b';  
        if (S_ISCHR(mode)) ftype = 'c';  
        if (S_ISFIFO(mode)) ftype = '|';  
  
  	sprintf(perms_buff, "%c%c%c%c%c%c%c%c%c%c %c%c%c", ftype,  
  	mode & S_IRUSR ? 'r' : '-',                                           
  	mode & S_IWUSR ? 'w' : '-',                                           
  	mode & S_IXUSR ? 'x' : '-',                                           
  	mode & S_IRGRP ? 'r' : '-',                                           
  	mode & S_IWGRP ? 'w' : '-',  
  	mode & S_IXGRP ? 'x' : '-',  
  	mode & S_IROTH ? 'r' : '-',  
  	mode & S_IWOTH ? 'w' : '-',  
  	mode & S_IXOTH ? 'x' : '-',  
  	mode & S_ISUID ? 'U' : '-',                                           
  	mode & S_ISGID ? 'G' : '-',  
  	mode & S_ISVTX ? 'S' : '-');  
  
        return (const char *)perms_buff;  
}  
int file_selecto_la(const struct direct   *entry)                       
{
     char *ptr;
     char *rindex(const char *s, int c );
     if ((strcmp(entry->d_name, ".")== 0) ||(strcmp(entry->d_name, "..") == 0))
       return (1);//false
 
				                                                         
    ptr = rindex(entry->d_name, '.');
    if ((ptr != NULL))// && ((strcmp(ptr, ".c") == 0) ||(strcmp(ptr, ".h") == 0) ||(strcmp(ptr, ".o") == 0) ))

		return (1);
    else
		return(0);
}
int file_selecto(const struct direct   *entry)                       
{
     char *ptr;
     char *rindex(const char *s, int c );
     if ((strcmp(entry->d_name, ".")== 0) ||(strcmp(entry->d_name, "..") == 0))
       return (0);//false
 
				                                                         
    ptr = rindex(entry->d_name, '.');
    if ((ptr != NULL))//&& ((strcmp(ptr, ".c") == 0) ||(strcmp(ptr, ".h") == 0) ||(strcmp(ptr, ".o") == 0) ))

		return (1);
    else
		return(0);
}

int file_select(const struct direct *entry)                                   
{
    if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0))
        return (1);
    else
        return (1);
}
int ls_a(char ** arguments)                                                            
{
                int count,i,status;
                struct direct **files;

                if(!getcwd(pathname, sizeof(pathname)))
                {
                	perror("Error getting pathname\n");
                	exit(0);
                }

                //printf("Current Working Directory = %s\n",pathname);
                count = scandir(pathname, &files, file_select, alphasort);

                                                                            
                if(count <= 0)
                {
                perror("No files in this directory\n");
                exit(0);
            	}
    
               // printf("Number of files = %d\n",count);
                for (i=1; i<count+1; ++i)
                printf("%s ",files[i-1]->d_name);
                printf("\n"); /* flush buffer */
                //exit(0);
            	status =1;
                return status;
}
int ls_la(char ** arguments)   //execute lp                                                       
{     
	  int status;
      int count,i;  
      struct direct **files;  
      struct stat statbuf;  
      char datestring[256];  
      struct passwd pwent;  
      struct passwd *pwentp;  
      struct group grp;  
      struct group *grpt;  
      struct tm time;  
      char buf[1024];  
  
      if(!getcwd(pathname, sizeof(pathname)))
      {  
         perror("Error getting pathnamen");
         exit(0);
      }  
  
      count = scandir(pathname, &files, file_selecto_la, alphasort);  
  
      if(count > 0)  
           {  
               printf("total %d\n",count);  
  
                  for (i=0; i<count; ++i)  
                   {  
                     if (stat(files[i]->d_name, &statbuf) == 0)  
                         {  
                                                                                            /* Print out type, permissions, and number of links. */  
                            printf("%10.10s", get_perms(statbuf.st_mode));                      /* File mode (type, perms) */       
                            printf(" %lu", statbuf.st_nlink);                                     /* Number of links */
  
                            if (!getpwuid_r(statbuf.st_uid, &pwent, buf, sizeof(buf), &pwentp))  
                                printf(" %s", pwent.pw_name);  
                            else  
                                printf(" %d", statbuf.st_uid);                                       /* User ID of the file's owner */
  
                            if (!getgrgid_r (statbuf.st_gid, &grp, buf, sizeof(buf), &grpt))  
                                printf(" %s", grp.gr_name);  
                            else  
                                printf(" %d", statbuf.st_gid);                                       /* Group ID of the file's group */  
  
                                                                                                 /* Print size of file. */  
                            printf(" %5d", (int)statbuf.st_size);  
                                                                                  
                            localtime_r(&statbuf.st_mtime, &time);                               /* Time of last data modification */
                                                                                                 /* Get localized date string. */  
                            strftime(datestring, sizeof(datestring), "%F %T", &time);  
  
                            printf(" %s %s\n", datestring, files[i]->d_name);  
                         }  
  
                                      free (files[i]);  
                    }  
  
                                      free(files);  
           }                                                                                               
                                      status=1;
                                      return status;
}
int ls_l(char ** arguments)   //execute lp                                                       
{     
	  int status;
      int count,i;  
      struct direct **files;  
      struct stat statbuf;  
      char datestring[256];  
      struct passwd pwent;  
      struct passwd *pwentp;  
      struct group grp;  
      struct group *grpt;  
      struct tm time;  
      char buf[1024];  
  
      if(!getcwd(pathname, sizeof(pathname)))
      {  
         perror("Error getting pathnamen");
         exit(0);
      }  
  
      count = scandir(pathname, &files, file_selecto, alphasort);  
  
      if(count > 0)  
           {  
               printf("total %d\n",count);  
  
                  for (i=0; i<count; ++i)  
                   {  
                     if (stat(files[i]->d_name, &statbuf) == 0)  
                         {  
                                                                                            /* Print out type, permissions, and number of links. */  
                            printf("%10.10s", get_perms(statbuf.st_mode));                      /* File mode (type, perms) */       
                            printf(" %lu", statbuf.st_nlink);                                     /* Number of links */
  
                            if (!getpwuid_r(statbuf.st_uid, &pwent, buf, sizeof(buf), &pwentp))  
                                printf(" %s", pwent.pw_name);  
                            else  
                                printf(" %d", statbuf.st_uid);                                       /* User ID of the file's owner */
  
                            if (!getgrgid_r (statbuf.st_gid, &grp, buf, sizeof(buf), &grpt))  
                                printf(" %s", grp.gr_name);  
                            else  
                                printf(" %d", statbuf.st_gid);                                       /* Group ID of the file's group */  
  
                                                                                                 /* Print size of file. */  
                            printf(" %5d", (int)statbuf.st_size);  
                                                                                  
                            localtime_r(&statbuf.st_mtime, &time);                               /* Time of last data modification */
                                                                                                 /* Get localized date string. */  
                            strftime(datestring, sizeof(datestring), "%F %T", &time);  
  
                            printf(" %s %s\n", datestring, files[i]->d_name);  
                         }  
  
                                      free (files[i]);  
                    }  
  
                                      free(files);  
           }                                                                                               
                                      status=1;
                                      return status;
}
   
int ls(char ** arguments)
{
int count,i,status;
    struct direct **files;

    if(!getcwd(pathname, sizeof(pathname)))
    {                                      
        perror("Error getting pathname");
  		exit(0);
  	}

   // printf("Current Working Directory = %s\n",pathname);
    count = scandir(pathname, &files,file_selecto,alphasort);
    //printf("count %d\n",count);

    /* If no files found, make a non-selectable menu item */
    if(count <= 0)
    {
      perror("No files in this directory\n");
  	  exit(0);
    }
    //printf("Number of files = %d\n",count);
    for (i=1; i<count+1; ++i)                                                  
        printf("%s ",files[i-1]->d_name);
    printf("\n"); /* flush buffer */
    		status=1;
            return status;

}

