#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aio.h>
#include <errno.h>

int main(int argc, char * argv[])
{
	int i;
	int s;
	struct aiocb* cba;
	char** ba;
	char* stop;
	char next=1;
	if (argc<2)
	{
		printf("Too few params\n");
		exit(1);
	}
	cba=(struct aiocb*)calloc(argc-1, sizeof(struct aiocb));
	ba=(char**)calloc(argc-1, sizeof(char*));
	stop=(char*)calloc(argc-1, sizeof(char));
	for (i=0; i<argc-1; ++i)
	{
		stop[i]=0;
		memset(&cba[i], 0, sizeof(struct aiocb));
		int fd;
		sscanf(argv[i+1],"%d",&fd);
		if (fd<0)
		{
			perror("Open failure");
			exit(2);
		}
		cba[i].aio_fildes=fd;
		cba[i].aio_offset=0;
		ba[i]=(char*)calloc(101,sizeof(char));
		cba[i].aio_buf=ba[i];
		cba[i].aio_nbytes=100;
		cba[i].aio_reqprio=0;
		s=aio_read(&cba[i]);
		if (s<0)
		{
			perror("AIO read failure");
			exit(2);
		}
	}
	while (next)
	{
		next=0;
		for (i=0; i<argc-1; ++i)
		{
			if (stop[i]) continue;
			next=1;
			while (aio_error(&cba[i])==EINPROGRESS)
				sleep(1);
			int actRead=aio_return(&cba[i]);
			
			if (actRead==0)
			{
				stop[i]=1;
				printf("%d Nothing to read.\n",i);
				close(cba[i].aio_fildes);
			}
			else
			{
				if (actRead<0)
				{
					perror("AIO return failure");
					exit(2);
				}
				printf("%d %s\n\n",i,ba[i]);

			}
		}
	}
	return 0;
}
