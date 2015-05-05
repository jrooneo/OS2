/************************************************
 * $Author: o-rooneo $
 *
 * $Date: 2015/02/26 00:57:56 $
 *
 * $Log: consumer.c,v $
 * Revision 1.2  2015/02/26 00:57:56  o-rooneo
 * outputting without doubling up on lines
 *
 * Revision 1.1  2015/02/25 20:24:30  o-rooneo
 * Initial revision
 *
 * Revision 1.1  2015/02/16 05:53:08  o-rooneo
 * Initial revision
 *
 *
 ***********************************************/
 
#include "definitions.h"
#include "fileHandler.h"

char exe[20];
char err[50];
int shmid[BUFFERCOUNT+1];
char *shm[BUFFERCOUNT+1];

enum state { idle, want_in, in_cs };
int *turn;
int flag[18]; // Flag corresponding to each process in shared memory

static void signal_handler(int signum)
{
	int i;
	for(i=0; i<=BUFFERCOUNT; i++){
		shmdt(shm[i]);
	}
	perror("consumer killed from interrupt");
	exit(1);
}

int main(int argc, char **argv)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, signal_handler);
	FILE *fp;
	int status, process=atoi(argv[1]);
	int i, n = 18; //max consumer count
	bool rerun = false;
	char *shared[BUFFERCOUNT+1], temp[BUFFERSIZE];
	int bufferFlags[5];

	time_t t;
	srand((unsigned) time(&t));

	for(i=0;i<=BUFFERCOUNT;i++){	 //Buffers start at index 1 and go to BUFFERCOUNT
		shmid[i] = shmget(BASESHMKEY+i, BUFFERSIZE, 0700);
	}

	for(i=0; i<=BUFFERCOUNT; i++){
		if (shmid[i] == -1){ 
			snprintf(err, 50, "%c: Producer shm error", exe);
			perror(err);
			exit ( 1 );
		}
	}

	for(i=0;i<=BUFFERCOUNT;i++){
		shm[i] = (char *)(shmat(shmid[i],0,0));
		shared[i] = (char *)shm[i];
	}
	
	turn = &shared[0][9];
	*flag = shared[0][10];
	int j; // Local to each process
	do{
		do{
			flag[process] = want_in; // Raise my flag
			j = *turn; // Set local variable
			while ( j != process )
				j = ( flag[j] != idle ) ? *turn : ( j + 1 ) % n;
			// Declare intention to enter critical section
			flag[process] = in_cs;
			// Check that no one else is in critical section
			for ( j = 0; j < n; j++ )
				if ( ( j != process ) && ( flag[j] == in_cs ) )
					break;
		} while (( j < n ) || ( *turn != process && flag[*turn] != idle ));
			// Assign turn to self and enter critical section
			*turn = process;
			/***************** Critical Section Begin *********************/
			for (i = 1; i <= BUFFERCOUNT; i++ ){
				if(shared[0][i]==1 && (shared[0][i]=-1)){
					//while(shared[0][6]!=0) wait(rand()%4);
					//if(shared[0][6]=1){ //If we can set the variable
						fp = fileHandlerWrite();							
						fprintf(fp, "Consumer %i %s",process, shared[i]);
						fclose(fp);
						//shared[0][6]=0;
					//}
					rerun = false;
					shared[0][i]=0;
					break;
				}else{
					wait(rand()%3);
				}
			}
			/***************** Critical Section End ***********************/
			j = (*turn + 1) % n;
			while (flag[j] == idle)
			j = (j + 1) % n;
			// Assign turn to next waiting process; change own flag to idle
			*turn = j;
			flag[process] = idle;
			if(shared[0][0]==1){
				if(rerun){
					break;
				}else{
					rerun = true;
				}
			}
	} while (1);
	shmdt(shm);
	exit(0);
}

