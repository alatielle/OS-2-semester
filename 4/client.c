#include <stdio.h>
#include "mylib.h"

int main() {
	int a;
	while(1) {
		a=mygetch();
		if(a==ESC) break;
		if(a==ENTER) printf("get data\n");
	}
	return 0;
}
