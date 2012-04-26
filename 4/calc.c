#include <stdio.h>
#include <string.h>
#include "mylib.h"
#define ROW 17
#define COL 17

int world[ROW][COL]; //future shared mem
int world2[ROW][COL];

void init() {
	FILE *f;
	int x, y;
	memset(world, 0, ROW*COL);
	memset(world2, 0, ROW*COL);
	f = fopen("input2.txt", "r");
	if(f) {
		while(!feof(f)) {
			fscanf(f, "%d", &x);
			fscanf(f, "%d", &y);
			world[x][y]=1;
			world2[x][y]=1;
		}
		fclose(f);
	}

}

void nextGen() {
	int i,j,count;
	for(i = 0; i < ROW; i++ ) {
  		for( j = 0; j < COL; j++ ) {
			count=0;
			if ( world[ (i+ROW-1)%ROW ] [ (j+COL - 1)%COL ] == 1 ) count++;
    			if ( world [ i ] [ (j - 1 + COL) % COL ] == 1 ) count++; 
			if ( world [ (i + 1) % ROW ] [ (j - 1 + COL) % COL ] == 1 )  count++;
			if ( world [ (i - 1 + ROW) % ROW ] [ j ] == 1 )  count++;
			if ( world [ (i + 1) % ROW ] [ j ] == 1 )  count++;
			if ( world [ (i - 1 + ROW) % ROW ] [ (j + 1) % COL ] == 1 ) count++;
			if ( world [ i ] [ (j + 1) % COL ] == 1 ) count++;
			if ( world [ (i + 1) % ROW ] [ (j + 1) % COL ] == 1 ) count++;
   	 		if (( world[i][j]==1 && count==2 )||count==3) world2[i][j]=1;
			else world2[i][j]=0;
		}
	}
/*for (i=0; i<ROW; ++i)
 for (j=0; j<COL; ++j)
	world[i][j]=world2[i][j];*/
	memcpy(world, world2, ROW*COL*4);
//printf(" worldscr: %d\n",world[1][5]);
}

void display() {
	int x, y;
	for(x=0; x < ROW; x++) {
		for(y=0; y < COL; y++) {
			if (world[x][y]==0)
			{
				printf(" ");
			} else
				printf("X");
   			//printf("%d ", world[x][y]);
 		}
		puts("\n");
  	}
	puts("--------------------\n");
}

int main() {
	init();
	display();
	int a;
	while(1) {
		a=mygetch();
		if(a==ESC) break;
//TODO timer
		if(a==10) {
		nextGen();
		display();
		}
		
	}
	return 0;
}


