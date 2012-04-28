#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>      
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include "mylib.h"
#define ROW 17
#define COL 17

char *world;
char world2[ROW][COL];
char worldt[ROW][COL];
int size = ROW*COL;
int fp2c[2];
int fp2s[2];
int mustsend=0;
int sending=0;

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
	char* curworld;
	if (sending!=0)
	{
		curworld=worldt;
	} else
	{
		curworld=world;
	}
	write(fp2c[1],"H",1);
	int i,j,count;
	for(i = 0; i < ROW; i++ ) {
  		for( j = 0; j < COL; j++ ) {
			count=0;
			if ( curworld [ ATROW ( (i+ROW-1)%ROW )	ATCOL ( (j+COL-1)%COL ) ] == 1 ) count++;
    			if ( curworld [ ATROW ( i )		ATCOL ( (j-1+COL)%COL ) ] == 1 ) count++; 
			if ( curworld [ ATROW ( (i+1)%ROW )	ATCOL ( (j-1+COL)%COL ) ] == 1 ) count++;
			if ( curworld [ ATROW ( (i-1+ROW)%ROW )	ATCOL ( j ) ] == 1 ) count++;
			if ( curworld [ ATROW ( (i+1)%ROW )	ATCOL ( j ) ] == 1 ) count++;
			if ( curworld [ ATROW ( (i-1+ROW)%ROW )	ATCOL ( (j+1)%COL ) ] == 1 ) count++;
			if ( curworld [ ATROW ( i )		ATCOL ( (j+1)%COL ) ] == 1 ) count++;
			if ( curworld [ ATROW ( (i+1)%ROW )	ATCOL ( (j+1)%COL ) ] == 1 ) count++;
   	 		if (( curworld[ ATROW(i) ATCOL(j) ] == 1 && count==2 )||count==3) world2[i][j]=1;
			else world2[i][j]=0;
		}
	}
	if (mustsend!=0)
	{
		memcpy(worldt, world, size);
		mustsend=0;
		sending=1;
		write(fp2c[1],"A",1);
	}
	memcpy(curworld, world2, size);
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
	write(fp2s[1],"D",1);
}

void sahandler(int signo, siginfo_t *info, void *context) {
	nextGen();
	alarm(1);
}

int main() {
	int fd;
	int msize = size; //ROW*COL

	void cleanmem() {
		munmap(world, msize);
		close(fd);
		shm_unlink("world");
	}

	void closepipe() {
		close(fp2c[0]);
		close(fp2c[1]);
		close(fp2s[0]);
		close(fp2s[1]);
	}

/*----shared memory----*/
	long int pgs = sysconf(_SC_PAGESIZE);
	if ( (fd = shm_open("world", O_CREAT|O_RDWR , 0777)) == -1 ) {
		perror("shm_open");
		return 1;
	}
	if( msize%pgs != 0 ) {
		msize -= msize%pgs - pgs;
	}
	if ( ftruncate(fd, msize) == -1 )  {
		perror("ftruncate");
		close(fd);
		shm_unlink("world");
		return 1;
	}
	world = mmap(0, msize, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);
	if ( world == (char*)-1 ) {
		close(fd);
		shm_unlink("world");
		perror("mmap");
		return 1;
	}
	memset(world, 0, size);
/*----pipe----*/
	if ( pipe2(fp2c, O_NONBLOCK) < 0 )
	{
		perror("Can't create pipe");
		cleanmem();
		return 1;
	}
	if ( pipe2(fp2s, O_NONBLOCK) < 0 )
	{
		perror("Can't create pipe");
		cleanmem();
		return 1;
	}
/*----fork----*/	
	pid_t p = fork();
	if(p==-1) {
		perror("Can't fork");
		cleanmem();
		closepipe();
		return 1;
	}
	if(p>0) {
		if( !init() ) {
			struct sigaction sa;
			struct sigaction previoussa;
			memset(&sa, 0, sizeof(sa));
			sa.sa_flags = SA_SIGINFO;
			sa.sa_sigaction = sahandler;
			if (sigaction(SIGALRM, &sa, &previoussa)<0) {
				perror("Sigaction failure");
				cleanmem();
				closepipe();
				return 1;
			}
			alarm(1);
			char c[1];
			int readres=read(fp2s[0],&c,1);
			if (readres<0)
			{
				perror("Pipe failure");
				cleanmem();
				closepipe();
				return 1;
			}
			if (readres>0)
				switch (c[0])
				{
					case 'E':
						cleanmem();
						closepipe();
						return 0;
					case 'R':
						mustsend=1;
						break;
					case 'D':
						memcpy(world, worldt, size);
						sending=0;
				}
			}
			//return 0;
		}
		else {
			return 1;
		}
	} 
	else {
//client part
		int flags = fcntl(0, F_GETFL, 0);
		fcntl(0, F_SETFL, flags | O_NONBLOCK);
		struct pollfd pfds[2];
		memset(pfds, 0, sizeof(pfds));
		pfds[0].fd = fp2c[0];
		pfds[0].events = POLLIN;
		pfds[1].fd = 0;
		pfds[1].events = POLLIN;
		int pollres;
		int readres;
		int curfd;
		char c[2];
		while(1) {
			pollres = poll(pfds, 2, 2000);
			if (pollres==0)
			{
				printf("Calculation unit failure: no heartbeat accepted\n");
				cleanmem();
				closepipe();
				return 1;
			}
			if (pollres<0)
			{
				perror("Poll failure");
				cleanmem();
				closepipe();
				return 1;
			}
			if (readres=read(fp2c[0],&c,1)<0)
			{
				perror("Read from pipe failure");
				cleanmem();
				closepipe();
				return 1;
			}
			if (readres>0)
			{
				switch (c[0])
				{
				case 'E':
					printf("Client unit exiting with reason: command from calculation unit");
					cleanmem();
					closepipe();
					return 0;
				case 'A':
					display();
					break;
				case 'H':
					break;
				}
			}
			if (pollres>=2 || (pollres==1 && readres<=0))
			{
				char c=mygetch();
				switch (c)
				{
				case ESC:
					printf("Client unit exiting with reason: command from console");
					write(fp2s[1],"E",1);
					cleanmem();
					closepipe();
					return 0;
				case ENTER:
					write(fp2s[1],"R",1);
				}
			}		
		}
	}
}


