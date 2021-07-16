#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<time.h>

//Function that takes two adresses of integers as parameters and swaps their pointed values 
// if needed, so that a<b
void orderUp(int* a, int* b){
	if(*a>*b){
		*a = *a+*b;
		*b = *a-*b;
		*a = *a-*b;
	}
}

int main(int argc, char* argv[]){
	//Seed randomizer
	srand(time(NULL));
	//fifos for communication between a and b
	mkfifo("a2b", 0777);
	mkfifo("b2a", 0777);
	//file descriptor
	int ab, ba;
	ba = 1;
	ba= ba+2;
	//id`s of children
	int ida;
	//intervals for picking a random number
	int lowerLimit, upperLimit, randn;
	ida=fork();
	if(ida==-1) perror("Bad fork() first child");
	if(ida ==0){//we are in child A
		printf("We are in child A.\nGive lower and upper limit to choose a random number,  please->  ");
		scanf("%d %d", &lowerLimit, &upperLimit);
		//Make sure lowerLimit < upperLimit; if not, swap them
		orderUp(&lowerLimit, &upperLimit);
		//Write to a2b fifo (communicate limits to childB)
		ab = open("a2b", O_WRONLY);
		if(ab == -1) perror("Error opening a2b in childA");	
		if(write(ab, &lowerLimit, sizeof(int)) == -1) 
			perror("Error with writing in childA");
		if(write(ab, &upperLimit, sizeof(int)) == -1)
			perror("Error with writing in childA");
		close(ab);
		//Read the chosen pseudorandom number from child A
		ba = open("b2a", O_RDONLY);
		if(ba==-1) perror("Error opening b2a in childA");
		if(read(ba, &randn, sizeof(int)) == -1) 
			perror("Error reading b2a in childA)");
		printf("The Chosen One is: %d\n", randn);	
		exit(0);
	}
	else{
		int idb = fork();
		if(idb==-1) perror("Bad fork() second child");
		if(idb==0){//we are in child B
			printf("We are in child B\n");
			srand(time(NULL));
			//Read from child A
			ab = open("a2b", O_RDONLY);	
			if(ab==-1) perror("Error opening a2b in childB");
			if(read(ab, &lowerLimit, sizeof(int))==-1)
				perror("Error reading in childB");
			if(read(ab, &upperLimit, sizeof(int))==-1)
				perror("Error reading in childB");
			printf("Lower limit: %d\nUpper limit: %d\n", lowerLimit, upperLimit);
			close(ab);
			//Calculate pseudorandom to be between lowerLimit and upperLimit
			randn = (rand() + lowerLimit)%upperLimit;
			//Send chosen pseudorandom to child A
			ba = open("b2a", O_WRONLY);
			if(ba==-1) perror("Error opening b2a in childB");
			if(write(ba, &randn, sizeof(int))==-1) perror("Error writing in childB");
			close(ba);
			exit(0);	
		}
		wait(0);
		wait(0);	
	}
	remove("a2b");
	remove("b2a");
	return 0;
}
