#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
	while (fork());
	return 0;
}
