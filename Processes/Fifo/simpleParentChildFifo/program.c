#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>


#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int main(int argc, char* argv[]){
	mkfifo("p2c", 0777);
	mkfifo("c2p", 0777);
	//File descriptors
	int pc, cp;
	//Problem vars
	int *a, sum;
	//A is a dynamic array holding two integers, simple as that.
	//Dont be fooled by pointers and calloc to think its something complicated!
	a = calloc(2, sizeof(int));
	int id=fork();
	if(id==-1) perror("Fork() problem");
	if(id==0){//Child
		pc = open("p2c", O_RDONLY);
		if(pc == -1) perror("Error with opening files in child");
		if(read(pc, a, sizeof(int)*2)==-1) perror("Error with read in child");
		printf("A is %d\nB is %d\n", *a, *(a+1));
		sum = *a+*(a+1);
		cp = open("c2p", O_WRONLY);
		if(write(cp, &sum, sizeof(int))==-1) perror("Error with write in child");
		if(pc == -1) perror("Error with opening files in child");
		close(pc);
		close(cp);
		free(a);
		exit(0);
	}
	//Parent
	printf("Read first number\n");
	scanf("%d", a);
	printf("Read second number\n");
	scanf("%d", a+1);
	pc = open("p2c", O_WRONLY);
	if(write(pc, a, sizeof(int)*2)==-1) perror("Error with write in parent");
	cp = open("c2p", O_RDONLY);
	//Very important: in similar cases parent should wait for calculations to be done
	//Before trying to read the result
	//in our case wait(0) here is redundant, since reading from child2parent will wait
	//first for having something to read, and implicitly wait for child to finish calculations 
	//and write to child2parent; but this is not always the case, and one should be careful
	wait(0);
	if(read(cp, &sum, sizeof(int))==-1) perror("Error with read in parent");
	printf("Sum of %d and %d is: %d.\n", *a, *(a+1), sum);
	free(a);
	remove("c2p");
	remove("p2c");
	return 0;
}
