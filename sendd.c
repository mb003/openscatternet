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

#include <btopush/btopush.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include "bluetooth/hci.h"
#include "include/bluez-libs-3.28/src/hci.c"
#include "include/bluez-libs-3.28/src/bluetooth.c"
#include "header.h"



void treeformation( btopush_ctx_t *btctx,btopush_dev_t *devs, int *devc )
{
   char addr[18],self_addr[18],tree_addr[18];
   FILE *fp,*conf;
   char fname[70]; 
   int i,dev_id,node_status=FREE_NODE,N=0,NDesc=-1;  
   bdaddr_t ba;

   if ((*devc = btopush_inq_objpush(devs)) <= 0) {
	fprintf(stderr, "could not find objpush capable devices\n");
        hci_devba(0, &ba);
        ba2str(&ba, self_addr);
        conf = fopen(NODE_STATUS_FILE,"w");
        fprintf(conf,"%s %s %d %d %d\n",self_addr,self_addr,FREE_NODE,N,NDesc);
        fclose(conf);
	return;
   }

      if( (conf = fopen(NODE_STATUS_FILE,"r")) > 0 ) 
      {
        fscanf(conf,"%s %s %d %d %d\n",self_addr,tree_addr,&node_status,&N,&NDesc);
        fclose(conf);
      }

   for(i=0; i< *devc; i++ )
   {  
      
      /* initialize bluetooth */
      btopush_init(btctx);

      //ba2str(&(devs->addr), addr);
      dev_id = hci_get_route(NULL);
      hci_devba(0, &ba);
      ba2str(&ba, self_addr);
      ba2str(&((devs+i)->addr), addr);
      ba2str(BDADDR_ANY, tree_addr);

      

      strcpy(fname,Init); 
      strcat(fname,self_addr);

      conf = fopen(NODE_STATUS_FILE,"w");          
      fprintf(conf,"%s %s %d %d\n",self_addr,tree_addr,node_status,N,NDesc);
      fclose(conf);

      fp = fopen(fname,"w");
      fprintf(fp,"%s %s %d %d\n",self_addr,tree_addr,node_status,N);
      fclose(fp);
		    
	if (btopush_attach_dev(btctx, devs+i) != BTOPUSH_SUCCESS) {
	    fprintf(stderr, "%s could not set device\n", addr);
	    return;
	}    
	
	if (btopush_open(btctx) != BTOPUSH_SUCCESS) {
	    fprintf(stderr, "%s could not open connection\n", addr);
            return;
	}	

	if (btopush_connect(btctx, "prijsobject") != BTOPUSH_SUCCESS) {
	    fprintf(stderr, "%s could not connect\n", addr);
	    return;
	}
	
        fprintf(stdout, "%s start sending %s\n", addr, fname);
	if (btopush_open_file(btctx, fname) != BTOPUSH_SUCCESS) {
	   fprintf(stderr, "could not open file: %s\n", fname);
	   goto disc;
        }
	    
	if (btopush_push_stream(btctx) != BTOPUSH_SUCCESS) {
	   if (btctx->req_state == BTOPUSH_REQS_TIMEOUT) {
	      fprintf(stderr, "%s connection timed out\n", addr);
	      btopush_close_file(btctx);
	      goto disc;
	   } 
           else {
	      fprintf(stderr, "%s cancelled\n", addr);
	      btopush_close_file(btctx);
	      goto disc;
	   }
	 } 
         else {
	    fprintf(stdout, "%s stream succesfull\n", addr); 
	 }
         
         btopush_close_file(btctx);
disc:
	 btopush_disconnect(btctx);
   }
   conf = fopen(NODE_STATUS_FILE,"w");          
   fprintf(conf,"%s %s %d %d %d\n",self_addr,tree_addr,node_status,N,NDesc);
   fclose(conf);
}

int main()
{
   btopush_ctx_t btctx;
   btopush_dev_t devs[BTOPUSH_MAX_DEV];
   char * rmaddr = NULL;
   char addr[18];
   int devc, i=0,sent;
   FILE *sent_count;

/* initialize bluetooth */
   btopush_init(&btctx);

   for( ; ; )
   {
     treeformation(&btctx,&devs[0],&devc);
     do 
     {
       system("../recv/recvd");
       sent_count = fopen(SENT_COUNT_FILE,"r");
       fscanf(sent_count,"%d",&sent);
       fclose(sent_count);
       sleep(30);
     }while(sent=0)   
   }
   return 0;
}
