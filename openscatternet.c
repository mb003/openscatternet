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

#define FREE_NODE 0
#define ROOT_NODE 1
#define NON_ROOT_NODE 2


void treeformation( btopush_ctx_t *btctx,btopush_dev_t *devs, int *devc )
{
   char addr[18];
   FILE *fp;
   char fname[70]; 
   int i;  
   
   if ((*devc = btopush_inq_objpush(devs)) <= 0) {
	fprintf(stderr, "could not find objpush capable devices\n");
	exit(1);
   }
   
   strcpy(fname,"Init");
   for(i=0; i< *devc; i++ )
   {         
      /* initialize bluetooth */
      btopush_init(btctx);

      ba2str(&(devs->addr), addr);
      strcat(fname,addr);
      fp = fopen(fname,"w");
      fprintf(fp,"Hello World !");
      fclose(fp);
		    
	if (btopush_attach_dev(btctx, devs) != BTOPUSH_SUCCESS) {
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
            /* to add code for merging procedure */ 
	 }
	 btopush_close_file(btctx);
disc:
	 btopush_disconnect(btctx);
   }
}

int main()
{
   btopush_ctx_t btctx;
   btopush_dev_t devs[BTOPUSH_MAX_DEV];
   char * rmaddr = NULL;
   char addr[18];
   int devc, i=0;
   ba2str(BDADDR_LOCAL,addr);
   printf("\n%s",addr);

      /* initialize bluetooth */
   btopush_init(&btctx);

   //for( ; ; )
   //{
      treeformation(&btctx,&devs[0],&devc);
      //sleep(300);
   //}
   return 0;
}
