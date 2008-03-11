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
#include <time.h>
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
#include "header.h"


int main (void)
{
       DIR *dp;
       struct dirent *ep;
       char filename[MAX_FILES][25],recv_addr[18],self_addr[18],sent_addr[18],tree_addr[18],recv_tree_addr[18];
       FILE *fp,*sent,*recv,*node,*list;
       int i=0,j,flag,dev_id,self_node_status,recv_node_status,N;
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

       for( i=0; i<=j; i++)
       {
         sent = fopen("../sent.list","r");

/* Read the Node Address from the received filename[i] */
         fp = fopen(filename[i],"r");
         fgets(recv_addr,18,fp);
         fseek(fp,1,1);
         fgets(recv_tree_addr,18,fp);
         fscanf(fp,"%d %d",&recv_node_status,&N);
         fclose(fp);

/* Searching the sent file for Node Address read from the received filename[i] */   
         while((fscanf(sent,"%s",sent_addr)) != EOF)
         { 
           
           if((flag = strcmp(sent_addr,recv_addr)) == 0 )
           {             

/* Reading the contents from node_status.conf */ 
             node = fopen("../node_status.conf","r");
             fgets(self_addr,18,fp);
             fseek(fp,1,1);
             fgets(tree_addr,18,node);
             fscanf(node,"%d %d",&self_node_status,&N);
             fclose(node);    
             

/* Tearing down the link if both nodes belong to same tree */
             if( strcmp(tree_addr,recv_tree_addr) == 0 && 
                (strcmp(tree_addr,"00:00:00:00:00:00"))!=0 ) break;

/* Checking for case A2   */
             else if( recv_node_status == FREE_NODE )
             { 
               if(self_node_status == FREE_NODE)
               { 
                  srand((unsigned int)time( NULL ));
                  if(rand()%2)
                  {
                    self_node_status = ROOT_NODE;
                    strcpy(tree_addr,self_addr);
                    N=1;
                    node = fopen("../node_status.conf","w");
                    fprintf(node,"%s %s %d %d",self_addr,tree_addr,self_node_status,N);
                    fclose(node);                    
                  }
                  else
                  {
                    self_node_status = NON_ROOT_NODE;
                    strcpy(tree_addr,recv_addr);
                    N=1;
                    node = fopen("../node_status.conf","w");
                    fprintf(node,"%s %s %d %d",self_addr,tree_addr,self_node_status,N);
                    fclose(node);
                  }
               }
               else if(self_node_status == NON_ROOT_NODE)
               {
               }
               else
               {
               }
               break;
             }
             else {}

           } 
         }
/* Deleting Files after use */
         //unlink(filename[i]);   
         fclose(sent);
       }


       return 0;
}
