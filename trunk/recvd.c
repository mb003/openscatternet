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
#include <btopush/btopush.h>
#include <bluetooth/hci.h>
#include "include/bluez-libs-3.28/src/hci.c"
#include "include/bluez-libs-3.28/src/bluetooth.c"
#include "include/btopush-1.0/btopush/sdp.c"
#include "bluetooth/hci_lib.h"
#include "header.h"



int main (void)
{
  DIR *dp;
  btopush_dev_t devs[BTOPUSH_MAX_DEV];btopush_ctx_t btctx;
  struct dirent *ep;
  char filename[MAX_FILES][25],recv_addr[18],self_addr[18],sent_addr[18];
  char tree_addr[18],recv_tree_addr[18],temp_addr[18],fname[30];
  FILE *fp,*sent,*recv,*node,*list,*send;
  int i=0,j,a,flag,dev_id,self_node_status,recv_node_status,N,ch,devc;
  bdaddr_t ba,ba1;
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

/* Reading the contents from node_status.conf */ 
    node = fopen(NODE_STATUS_FILE1,"r");
    fgets(self_addr,18,fp);
    fseek(fp,1,1);
    fgets(tree_addr,18,node);
    fscanf(node,"%d %d",&self_node_status,&N);
    fclose(node);    
             
/* Searching the sent file for verifying Node Address, 
read from the received filename[i]                  */   
    while((fscanf(sent,"%s",sent_addr)) != EOF)
    { 
       
      if((flag = strcmp(sent_addr,recv_addr)) == 0 )
      {             

/* Reading the contents from node_status.conf */ 
        node = fopen(NODE_STATUS_FILE1,"r");
        fgets(self_addr,18,fp);
        fseek(fp,1,1);
        fgets(tree_addr,18,node);
        fscanf(node,"%d %d",&self_node_status,&N);
        fclose(node);    
            

/* Tearing down the link if both nodes belong to same tree */
        if( strcmp(tree_addr,recv_tree_addr) == 0 && 
        (strcmp(tree_addr,"00:00:00:00:00:00"))!=0 ) break;
 

                                                         /* Update Message as Response for Init,
                                                            Checking for case A2  */
        else if( recv_node_status == FREE_NODE )
        { 
          if(self_node_status == FREE_NODE)
          { 
            srand((unsigned int)time( NULL ));
            if(rand()%2)
            {
              self_node_status = ROOT_NODE;
              strcpy(tree_addr,self_addr);
              N++;
              node = fopen(NODE_STATUS_FILE1,"w");
              fprintf(node,"%s %s %d %d",self_addr,tree_addr,self_node_status,N);
              fclose(node);
              node = fopen(TOPOLOGY_FILE,"w");
              fprintf(node,"%s\n",recv_addr);
              fclose(node);                    
            }
            else
            {
              self_node_status = NON_ROOT_NODE;
              strcpy(tree_addr,recv_addr);
              N++;
              node = fopen(NODE_STATUS_FILE1,"w");
              fprintf(node,"%s %s %d %d",self_addr,tree_addr,self_node_status,N);
              fclose(node);
            }
            /*         
            str2ba(recv_addr,&ba);
            str2ba(self_addr,&ba1);
	    ch = btopush_get_channel(&ba1, &ba);
	    if (ch != BTOPUSH_ERROR) {
	    memcpy(&devs.addr, (bdaddr_t *) recv_addr, sizeof(bdaddr_t));
	    devs.channel = ch;
            printf("\n%d\n",ch);
            }
            */
            if ((devc = btopush_inq_objpush(devs)) <= 0) 
            {
	      fprintf(stderr, "could not find objpush capable devices\n");
              return;
            }

            for(a=0;a<BTOPUSH_MAX_DEV;a++)
            {
              ba2str(&(devs[a].addr),temp_addr);
              if( strcmp(recv_addr,temp_addr) == 0 )break;
            }                                                   
                                                                   
            strcpy(fname,"Update"); 
            strcat(fname,self_addr);
	    send = fopen(fname,"w");          
            fprintf(send,"%s %s %d %d\n",self_addr,tree_addr,self_node_status,N);
            fclose(send);
            btopush_init(&btctx);
 
            if (btopush_attach_dev(&btctx,(devs+a)) != BTOPUSH_SUCCESS)
            {
	      fprintf(stderr, "%s could not set device\n", recv_addr);
	      return;
	    }    
	
	    if (btopush_open(&btctx) != BTOPUSH_SUCCESS)
            {
	      fprintf(stderr, "%s could not open connection\n", recv_addr);
	      return;
            }	

	    if (btopush_connect(&btctx, "prijsobject") != BTOPUSH_SUCCESS) 
            {
	      fprintf(stderr, "%s could not connect\n", recv_addr);
	      return;
	    }
	    send = fopen(fname,"r");
            fprintf(stdout, "%s start sending %s\n", recv_addr, fname);
	    if (btopush_open_file(&btctx, fname) != BTOPUSH_SUCCESS) 
            {
	      fprintf(stderr, "could not open file: %s\n", fname);
	      goto disc;
            }
	    
	    if (btopush_push_stream(&btctx) != BTOPUSH_SUCCESS) 
            {
	      if (btctx.req_state == BTOPUSH_REQS_TIMEOUT) 
              {
	        fprintf(stderr, "%s connection timed out\n", recv_addr);
	        btopush_close_file(&btctx);
	        goto disc;
	      } 
              else 
              {
	        fprintf(stderr, "%s cancelled\n", recv_addr);
	        btopush_close_file(&btctx);
	        goto disc;
	      }
	    } 
            else 
            {
	      fprintf(stdout, "%s stream succesfull\n", recv_addr); 
	    }
            disc: 
	      btopush_disconnect(&btctx);fclose(send);   
	  }                                             /*    End of case A2    */ 
        }   
                                                 
        if(recv_node_status == NON_ROOT_NODE || recv_node_status == ROOT_NODE )        
        {                                              
                                                       /*Response to Update Message*/
            self_node_status = NON_ROOT_NODE;
            if( recv_node_status == NON_ROOT_NODE ) 
            {
              self_node_status = ROOT_NODE;
              node = fopen(TOPOLOGY_FILE,"w");
              fprintf(node,"%s\n",recv_addr);
              fclose(node);
            }
            node = fopen("node_status.conf","w");     
            fprintf(node,"%s %s %d %d\n",self_addr,recv_tree_addr,self_node_status,N);
            fclose(node);
            break;
        }
      } 
    }                                            /* while loop Ends              */
    //unlink(filename[i]);                       /* Deleting Files after use     */
    fclose(sent);
  }                                              /* for loop Ends                */
  return 0;
}
