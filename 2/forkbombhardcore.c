#include <unistd.h>
#include <stdio.h>

int main() {
	if(!fork()) {
		if(setpgid(0, 0) == -1) {
			perror("Can't set PGID\n");
			return 1;
		}
		pid_t pgid = getpgid(0);
		if(pgid == -1) {
			perror("Can't get PGID\n");
			return 1;
		}
		if(printf("%d", pgid) < 0) {
			return 1;
		}
		while(!fork());
	}
	return 0;
}