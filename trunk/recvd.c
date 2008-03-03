#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#define MAX_FILES 10

int main (void)
{
       DIR *dp;
       struct dirent *ep;
       char filename[MAX_FILES][10],buffer[100];
       FILE *fp;
       int i=0,j;
       dp = opendir ("./");
       if (dp != NULL)
         {
           while (ep = readdir (dp))
           if( strcmp(ep->d_name,".") != 0  && strcmp(ep->d_name,"..") != 0 )
           {
             strcpy(filename[i],ep->d_name);
             //puts (filename[i]);//ep->d_name);
             i++;
           }
           (void) closedir (dp);
         }
       else
         perror ("Couldn't open the directory");
       j=i-1;
       for( i=0; i<j; i++)
       {
         fp = fopen(filename[i],"r");
         fscanf(fp,"%s",buffer);
         puts(filename[i]);
         puts(buffer);
         fclose(fp);
       }
       return 0;
}
