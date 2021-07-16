#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>


int main(int argc,char*  argv[]){
	int a, b, sum;
	//Declare pipes
	int c2p[2], p2c[2];
	//Init pipes
	if(pipe(c2p) == -1) return 2;
	if(pipe(p2c) == -1) return 2;
	//Create child
	int id = fork();
	if(id == -1) return 3;
	if(id==0){//Child will be here
		close(c2p[0]);
		close(p2c[1]);
		printf("Child says: Whats up, crip?\n");
		if(read(p2c[0], &a, sizeof(int))==-1) return 4;
		if(read(p2c[0], &b, sizeof(int))==-1) return 4;
		sum = a+b;
		if(write(c2p[1], &sum, sizeof(int))==-1) return 5;
		//Close pipe ends
		close(c2p[1]);
		close(p2c[0]);
		close(0);
	}
	//Parent will be here
	close(c2p[1]);
	close(p2c[0]);
	printf("Parent says: It ain`t blue, it ain`t true...\n");
	printf("Read first number\n");	
	scanf("%d", &a);
	printf("Read second number\n");
	scanf("%d", &b); 
	if(write(p2c[1], &a, sizeof(int)) == -1 ) return 5;
	if(write(p2c[1], &b, sizeof(int)) == -1 ) return 5;
	wait(0);
	if(read(c2p[0], &sum, sizeof(int)) == -1) return 4;
	printf("Sum of %d and %d is: %d!\n", a, b, sum);
	close(c2p[0]);
	close(p2c[1]);
	return 0;
}
