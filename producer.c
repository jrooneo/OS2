/************************************************
 * $Author: o-rooneo $
 *
 * $Date: 2015/02/26 00:58:27 $
 *
 * $Log: producer.c,v $
 * Revision 1.3  2015/02/26 00:58:27  o-rooneo
 * added full/empty checks to buffers. added break so that a sleep happens after every buffer fill
 *
 * Revision 1.2  2015/02/18 15:28:17  o-rooneo
 * Shared memory and signal handling implemented
 *
 * Revision 1.1  2015/02/16 05:54:27  o-rooneo
 * Initial revision
 *
 ***********************************************/

#include "fileHandler.h"
#include "definitions.h"

int shmid[BUFFERCOUNT+1];
char *shm[BUFFERCOUNT+1];

static void signal_handler(int signum)
{
	int i;
	for(i=0; i<=BUFFERCOUNT; i++){
		shmdt(shm[i]);
	}
	fprintf(stderr, "Producer was killed by signal number %i.\n", signum);
	exit(1);
}

int main(int argc, char **argv)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, signal_handler);
	int i, j;
	char *fgetsReturn;
	FILE* fp;
	char *shared[BUFFERCOUNT+1];
	time_t t;
	
	fp = fileHandlerRead();
	srand((unsigned) time(&t));
	
	for(i=0;i<=BUFFERCOUNT;i++){	 //Buffers start at index 1 and go to BUFFERCOUNT
		shmid[i] = shmget(BASESHMKEY+i, BUFFERSIZE, 0700);
	}
	for(i=0;i<=BUFFERCOUNT;i++){
		if (shmid[i] == -1){ 
			fprintf(stderr, "%s: Producer shm error", argv[0]);
			return 1;
		}
	}

	for(i=0;i<=BUFFERCOUNT;i++){
		shm[i] = (char *)(shmat(shmid[i],0,0));
		shared[i] = (char *)shm[i];
	}
	for(i=0;i<BUFFERCOUNT+2;i++){
		shared[0][i]=0;
	}
	/***************** Critical Section Begin *********************/
	while(shared[0][0]!=1){
		for(i=1;i<=BUFFERCOUNT;i++){
			if(shared[0][i]==0){
				for(j=0;j<BUFFERSIZE;j++) shared[i][j] = '\0'; //ensure zero'd array before writing
				fgetsReturn = fgets(shared[i], BUFFERSIZE-1, fp);
				if(fgetsReturn == NULL){
					shared[0][0]=1; //indicate EOF found
					break;
				}
				fprintf(stderr, "PROD BUFF %i: %s", i, shared[i]);
				shared[0][i]=1;
				break;
			}else{
				sleep(rand()%3);
			}
		}
	}
	/***************** Critical Section End ***********************/
	for(i=0; i<=BUFFERCOUNT; i++){
		shmdt(shm[i]);
	}
	return 0;
}
	//shared[0] is an array of flags for buffer 0=empty 1=full -1=in-use
	//shared[0][1-5]=flags for full on corresponding shm array
	//shared[0][6] = file in use
	//shared[0][7] = EOF encountered
