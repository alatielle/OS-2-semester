#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aio.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#define buf_size 100

struct aiocb* cba;
struct aiocb** cblist;
char** ba;
char* stop;
int maxInd;

void freeAll()
{
	int i;
	free(cba);
	free(cblist);
	free(stop);
	if (ba!=NULL)
		for (i=0; i<maxInd; ++i)
			free(ba[i]);
	free(ba);
}

int main(int argc, char * argv[])
{
	int i;
	int s;
	char next=1;
	if (argc<2)
	{
		printf("Too few params\n");
		exit(1);
	}
	maxInd=argc-1;
	cba=(struct aiocb*)calloc(argc-1, sizeof(struct aiocb));
	cblist=(struct aiocb**)calloc(argc-1, sizeof(struct aiocb*));
	ba=(char**)calloc(argc-1, sizeof(char*));
	stop=(char*)calloc(argc-1, sizeof(char));
	if (cba==NULL || cblist==NULL || ba==NULL || stop==NULL)
	{
		printf("Memory allocation error\n");
		freeAll();
	}
	for (i=0; i<argc-1; ++i)
	{
		stop[i]=0;
		memset(&cba[i], 0, sizeof(struct aiocb));
		int fd;
		sscanf(argv[i+1],"%d",&fd);
		if (fd<0)
		{
			perror("Open failure");
			freeAll();
			exit(2);
		}
		cba[i].aio_fildes=fd;
		cba[i].aio_offset=0;
		ba[i]=(char*)calloc(buf_size+1,sizeof(char));
		cba[i].aio_buf=ba[i];
		cba[i].aio_nbytes=buf_size;
		cba[i].aio_reqprio=0;
		cblist[i]=&cba[i];
		s=aio_read(&cba[i]);
		if (s<0)
		{
			perror("AIO read failure");
			freeAll();
			exit(2);
		}
	}
	while (next)
	{
		next=0;
		while (aio_suspend(cblist,argc-1,0)<0)
			if (errno==EINTR) continue;
				else break;
		for (i=0; i<argc-1; ++i)
		{
			if (stop[i]) continue;
			next=1;
			if (aio_error(&cba[i])==EINPROGRESS) continue;
			
			if (aio_error(&cba[i])!=0)
			{
				perror("AIO read error");
				freeAll();
				exit(2);
			}
			int actRead=aio_return(&cba[i]);
			
			if (actRead==0)
			{
				stop[i]=1;
				printf("Entry # %d: fd %d - nothing to read.\n",i ,cba[i].aio_fildes);
			}
			else
			{
				if (actRead<0)
				{
					perror("AIO return failure");
					freeAll();
					exit(2);
				}
				printf("Entry # %d: fd %d - %d bytes read.\n---------------\n",i, cba[i].aio_fildes,actRead);
				printf("%s\n\n",ba[i]);
				cba[i].aio_offset+=actRead;
				s=aio_read(&cba[i]);
				if (s<0)
				{
					perror("AIO read failure");
					freeAll();
					exit(2);
				}
			}
		}
	}
	freeAll();
	return 0;
}
