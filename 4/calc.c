#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>      
#include <fcntl.h> 
#include "mylib.h"
#define ROW 17
#define COL 17

int world2[ROW][COL];
int size = ROW*COL*4;

int init() {
	FILE *f;
	int x, y;
	memset(world2, 0, size);
	f = fopen("input2.txt", "r");
	if(f) {
		while(!feof(f)) {
			fscanf(f, "%d", &x);
			if(x>=ROW || x<0) {
				perror("Bad data file");
				return 1;
			}
			fscanf(f, "%d", &y);
			if(y>=COL || y<0) {
				perror("Bad data file");
				return 1;
			}
			world[x*ROW + y]=1;
			world2[x][y]=1;
		}
		fclose(f);
	}

}

#define ATROW ROW*
#define ATCOL +

void nextGen() {
	int i,j,count;
	for(i = 0; i < ROW; i++ ) {
  		for( j = 0; j < COL; j++ ) {
			count=0;
			if ( world [ ATROW ( (i+ROW-1)%ROW )	ATCOL ( (j+COL-1)%COL ) ] == 1 ) count++;
    			if ( world [ ATROW ( i )		ATCOL ( (j-1+COL)%COL ) ] == 1 ) count++; 
			if ( world [ ATROW ( (i+1)%ROW )	ATCOL ( (j-1+COL)%COL ) ] == 1 ) count++;
			if ( world [ ATROW ( (i-1+ROW)%ROW )	ATCOL ( j ) ] == 1 ) count++;
			if ( world [ ATROW ( (i+1)%ROW )	ATCOL ( j ) ] == 1 ) count++;
			if ( world [ ATROW ( (i-1+ROW)%ROW )	ATCOL ( (j+1)%COL ) ] == 1 ) count++;
			if ( world [ ATROW ( i )		ATCOL ( (j+1)%COL ) ] == 1 ) count++;
			if ( world [ ATROW ( (i+1)%ROW )	ATCOL ( (j+1)%COL ) ] == 1 ) count++;
   	 		if (( world[ ATROW(i) ATCOL(j) ] == 1 && count==2 )||count==3) world2[i][j]=1;
			else world2[i][j]=0;
		}
	}
	memcpy(world, world2, size);
}

void display() {
	int x, y;
	for(x=0; x < ROW; x++) {
		for(y=0; y < COL; y++) {
			if (world[ ATROW(x) ATCOL(y) ] == 0)
			{
				printf(" ");
			} else
				printf("X");
   			}
		puts("\n");
  	}
}

void sahandler(int signo, siginfo_t *info, void *context) {
	nextGen();
	alarm(1);
}

int main() {
/*----shared memory----*/
	int fd;
	char *world;
	long int pgs = sysconf(_SC_PAGESIZE);
	int msize = size; //ROW*COL*4
	if ( (fd = shm_open("world", O_CREAT|O_RDWR , 0777)) == -1 ) {
		perror("shm_open");
		return 1;
	}
	if(msize%pgs != 0) {
		msize -= msize%pgs - pgs;
	}
	if ( ftruncate(fd, msize) == -1 )  {
		perror("ftruncate");
		return 1;
	}
	world = mmap(0, msize, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);
	if ( world == (char*)-1 ) {
		perror("mmap");
		return 1;
	}
	memset(world, 0, size);
/*----pipe----*/
/*----fork----*/	
	pid_t p = fork();
	if(p>0) {
		if( !init() ) {
			struct sigaction sa;
			struct sigaction previoussa;
			memset(&sa, 0, sizeof(sa));
			sa.sa_flags = SA_SIGINFO;
			sa.sa_sigaction = sahandler;
			if (sigaction(SIGALRM, &sa, &previoussa)<0) {
				perror("Sigaction failure");
				return 1;
			}
			alarm(1);
			int a;
			while(1) {
				//TODO pipe
				a=mygetch();
				if(a==ESC) break;
			}
			return 0;
		}
		else {
			return 1;
		}
	} else {
//client part
	}
	
}


