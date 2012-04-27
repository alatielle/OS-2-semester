#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

int fp[2];

void sahandler(int signo, siginfo_t *info, void *context)
{
	if (info->si_signo==SIGCHLD && info->si_code==CLD_EXITED)
	{
		if (info->si_status==0)
		{
			printf("Child with pid %d successfuly exited\n", info->si_pid);
			write(fp[1],"0",1);
		} else
		{
			printf("Child with pid %d failed\n", info->si_pid);
			write(fp[1],"1",1);
		}
	} else if (info->si_signo==SIGCHLD)
	{
		if (info->si_code==CLD_KILLED)
		{
			printf("Child with pid %d were killed.\n", info->si_pid);
			write(fp[1],"2",1);
		} else if (info->si_code==CLD_DUMPED)
		{
			printf("Child with pid %d were terminated. Please see dump.\n", info->si_pid);
			write(fp[1],"?",1);
		}
	}
}

int main(int argc, char* argv[])
{
	int i;
	char reqstr[512];
	FILE* urlfd;
	struct sigaction sa;
	struct sigaction previoussa;
	if (pipe(fp)<0)
	{
		perror("Pipe can't be created");
	}
	if (argc<2)
	{
		perror("Too few params");
		exit(1);
	}
	if (argc>2)
	{
		perror("Too many params");
		exit(1);
	}
	memset(&sa, 0, sizeof(sa));
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = sahandler;
	if (sigaction(SIGCHLD, &sa, &previoussa)<0)
	{
		perror("Sigaction failure");
		exit(2);
	}
	urlfd=fopen(argv[1],"r");
	if (urlfd==NULL)
	{
		perror("Can't open main file");
		exit(2);
	}
	pid_t pid;
	i=0;
	while (fscanf(urlfd,"%s",&reqstr)>0)
	{
		++i;
		pid=fork();
		if (pid==-1)
		{
			perror("Can't fork!");
			exit(2);
		}
		if (!pid)
		{
			char filename[256];
			sprintf(filename,"%d.txt",i);
			FILE* outf=fopen(filename,"w");
			FILE* nulf=fopen("/dev/null","a");
			if (outf==NULL)
			{
				char error[512];
				sprintf(error,"Can't open file %s",filename);
				perror(error);
				fclose(nulf);
				exit(2);
			}
			if (nulf==NULL)
			{
				perror("Can't open /dev/null");
				fclose(outf);
				exit(2);
			}
			if (dup2(fileno(outf), 1) == -1 || dup2(fileno(nulf), 2) == -1)
			{
				perror("Dup error");
				fclose(outf);
				fclose(nulf);
				exit(2);
			}
			execlp("curl", "curl", reqstr, (char *)NULL);
		}
	}
	fclose(urlfd);
	char inp[1];
	while (i>0)
	{
		while (read(fp[0],&inp,1)<1)
		{
			if (errno==EINTR) continue;
			perror("Pipe failure");
			close(fp[0]);
			close(fp[1]);
			exit(2);
		}
		if (inp[0] == '0')
			printf("Success\n");
		else
			printf("Failure\n");
		--i;
	}
	printf("All done!\n");
	close(fp[0]);
	close(fp[1]);
	return 0;
}

