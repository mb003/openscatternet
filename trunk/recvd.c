/*
 *
 *  OpenScatternet 
 *  Scatternet Formation software
 *
 *  Copyright (C) 2008  Dinesh Uthayakumar <udinesh5@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation;
 *
 *  Software distributed under the License is distributed on an "AS
 *  IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 *  implied. See the License for the specific language governing
 *  rights and limitations under the License.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */


#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include "bluetooth/hci.h"
#include "../../bluez-libs-3.28/src/hci.c"
#include "../../bluez-libs-3.28/src/bluetooth.c"
#include "bluetooth/hci_lib.h"
#define MAX_FILES 20


int main (void)
{
       DIR *dp;
       struct dirent *ep;
       char filename[MAX_FILES][25],buffer[50],buffer1[50],addr[18];
       FILE *fp,*sent,*recv;
       int i=0,j,flag,dev_id;
       bdaddr_t ba;
       dp = opendir ("./");
       if (dp != NULL)
        {
           while (ep = readdir (dp))
           if( strcmp(ep->d_name,".") != 0  && strcmp(ep->d_name,"..") != 0 
              && strcmp(ep->d_name,"recvd") != 0)
           {
             strcpy(filename[i],ep->d_name);
             i++;
           }                    
           (void) closedir (dp);
        }
       else
         perror ("Couldn't open the directory");
       
       j=i-1;
       sent = fopen("../sent.list","r");
       for( i=0; i<j; i++)
       {
         fp = fopen(filename[i],"r");
         fgets(buffer,18,fp);
         //buffer[17]=NULL;
         while(fscanf(sent,"%s",addr) != NULL)
         { 
           printf("\n%s %s\n",addr,buffer);
           if((flag = strcmp(addr,buffer)) == 0 )
           {
             fgets(buffer,18,fp);
             fscanf(sent,"%s",buffer1);
             if(strcmp(buffer,buffer1) == 0) 
             {
               printf("\nCOOL !\n");
               break;
             }
           }   
         }
         
         fclose(fp);
       }
       fclose(sent);
       return 0;
}
