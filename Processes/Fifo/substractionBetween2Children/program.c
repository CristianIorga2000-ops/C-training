#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>


//Function that returns an integer pseudorandom number between min and max values passed as
//parameters
int chooseRand(int min, int max){
	return (rand()+min)%max;
}

int main(int argc, char* argv[]){
	srand(time(NULL)); // seed randomizer
	mkfifo("ab", 0777);
	mkfifo("ba", 0777);
	int ab, ba;
	int ida, idb;
	int n;
	printf("Choose n -> ");
	scanf("%d", &n);
	printf("N starts as %d\n", n);

	if((ida=fork()) == 0){
		//In child A
		ab = open("ab", O_WRONLY);
		if(write(ab, &n, sizeof(int)) == -1) perror ("error");
		close(ab);
		while(n>9){

			ba = open("ba", O_RDONLY);
			if(read(ba, &n, sizeof(int)) == -1) perror ("error");
			close(ba);	
			printf("A: N before is %d.\n", n);
			if(n<=9) exit(0);
			n-=1;
			printf("A: N is now %d.\n", n);
			ab = open("ab", O_WRONLY);
			if(write(ab, &n, sizeof(int)) == -1) perror("error");
			close(ab);

		}}

	if((idb=fork())== 0 ){
		//In child B
		while(n>9){
			ab = open("ab", O_RDONLY);
			if(read(ab, &n, sizeof(int)) == -1) perror("error");
			close(ab);		
			printf("B: N before is %d.\n", n);
			if(n<=9) exit(0);
			n-=4;
			printf("B: N is now %d.\n", n);
			ba = open("ba", O_WRONLY);			
			if(write(ba, &n, sizeof(int)) == -1) perror("error");
			close(ba);

		}
	}
	//The reason we keep the return values of both forks:
	//(to verify forking was succesfully done)
	if(ida == -1) perror("error");
	if(idb == -1) perror("error");
	wait(0);
	wait(0);
	remove("ab");
	remove("ba");
	return 0;
}
