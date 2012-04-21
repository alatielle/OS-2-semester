#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

struct sigaction *sa;
struct sigaction *previoussa;
int fp[2];

void sahandler(int signo, siginfo_t *info, void *context)
{
	if (info->si_signo==SIGCHLD && info->si_code==CLD_EXITED)
	{
		if (info->si_status==0)
		{
			printf("Child with pid %d successfuly exited\n", info->si_pid);
			write(fp[0],"0",1);
		} else
		{
			printf("Child with pid %d failed\n", info->si_pid);
			write(fp[0],"1",1);
		}
	} else if (info->si_signo==SIGCHLD)
	{
		if (info->si_code==CLD_KILLED)
		{
			printf("Child with pid %d were killed.\n", info->si_pid);
			write(fp[0],"2",1);
		} else if (info->si_code==CLD_DUMPED)
		{
			printf("Child with pid %d were terminated. Please see dump.\n", info->si_pid);
			write(fp[0],"?",1);
		}
	}
}

int main(int argc, char* argv[])
{
	int i;
	char* reqstr;
	pipe(fp);
	if (argc<2)
	{
		printf("Too few params\n");
		exit(1);
	}
	memset(sa, 0, sizeof(struct sigaction));
	sa->sa_flags = SA_SIGINFO;
	sa->sa_sigaction = sahandler;
	sigaction(SIGCHLD, sa, previoussa);
	pid_t pid;
	for (i=1; i<argc; ++i)
	{
		pid=fork();
		if (pid==-1)
		{
			printf("Can't fork!\n");
			exit(2);
		}
		if (!pid)
		{
			close(fp[0]);
			close(fp[1]);
			reqstr=argv[i];
			char* filename;
			sprintf(filename,"%d.txt",i);
			FILE* outf=fopen(filename,"w");
			FILE* nulf=fopen("/dev/null","a");
			if (outf==NULL)
			{
				printf("Can't open file %s\n",filename);
				exit(2);
			}
			if (nulf==NULL)
			{
				printf("Can't open /dev/null\n");
				exit(2);
			}
			if (dup2(fileno(outf), 1) == -1 || dup2(fileno(nulf), 2) == -1)
			{
				printf("Dup error\n");
				exit(2);
			}
			if (execlp("curl", "curl", reqstr, (char *)NULL) == -1)
			{
				printf("Exec No%d error\n",i);
				exit(2);
			}
			exit(0);
		}
	}
	close(fp[0]);
	i=1;
	char inp[1];
	while (i<argc-1)
	{
		if (read(fp[1],&inp,1)>1)
		{
			printf("Pipe failure\n");
			exit(2);
		}
		++i;
	}
	return 0;
}

