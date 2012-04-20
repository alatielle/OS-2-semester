#include <unistd.h>
#include <stdio.h>

int main() {
	setbuf(stdout, NULL);
	if(setpgid(0, 0) == -1) {
		perror("Can't set PGID");
		return 1;
	}
	pid_t pgid = getpgid(0);
	if(pgid == -1) {
		perror("Can't get PGID");
		return 1;
	}
	if(printf("%d\n", pgid) < 0) {
		return 1;
	}
	if(fork() == -1) {
		perror("Can't fork");
		return 1;
	}
	while(1) {
		fork();
	}
	return 0;
}