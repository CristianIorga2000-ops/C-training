#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <time.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

#define MAX_RANDOM_SIZE 100

/*This function will create an array of n pseudo-randomly generated numbers
 * and return it
 * input: int n (number of elements in array)
 * return: int* array (array with generated numbers)
 */
int* genRandNumbers(int n){
	srand(time(NULL));
	int* array;
	array = calloc(n, sizeof(int));
	for(int i=0; i<n; i++)
		*(array+i) = rand()%MAX_RANDOM_SIZE;
	return  array;
}

/*This functions calculates the sum of all elements in an integer array
* input: int* array, int arraySize
* return: int sum (sum of all elements)
*/
int getSum(int* array, int arraySize){
	int sum=0;
	for(int i=0; i<arraySize; i++)
	sum+=*(array+i);
	return sum;
}

int main(int argc, char *argv[]){
	//verify arguments are ok
	if(argc != 2) return 4;
	//convert argument to int
	int num;
	sscanf (argv[1],"%d",&num); 
	//create communication fifos
	mkfifo("c2p", 0777);
	mkfifo("p2c", 0777);
	//declare file descriptors
	int p2c, c2p;
	int id = fork();
	if(id == -1) return 2;
	if(id == 0){//we are in child
		printf("Child says hello\n");
		//Child must read from p2c fifo 
		p2c = open("p2c", O_RDONLY);
		if(p2c == -1) return 1;
		int *array;
		array = calloc(num, sizeof(int));
		//Read from file and verify it was ok
		if(read(p2c, array, sizeof(int)*num)==-1) return 2;
		int sum = getSum(array, num);
		printf("Sum was calculated to be %d\n", sum);
		//Child must write to c2p fifo
		c2p = open("c2p", O_WRONLY);
		if(c2p == -1) return 1;
		//Write to file and verify it was ok
		if(write(c2p, &sum, sizeof(int))==-1) return 3;
	        free(array);	
		exit(0);
	}
	//we are in parent
	//Child will never get here because of exit(0)
	printf("Parent says hello\n");
	int* array;
	//Generate rand array
	array = genRandNumbers(num);
	//Parent must write to p2c fifo
	p2c=open("p2c", O_WRONLY);
	if(p2c == -1) return 1;
	if(write(p2c, array, sizeof(int)*num)==-1) return 3;
	int sum;
	//Parent must read from c2p fifo
	c2p= open("c2p", O_RDONLY);
	if(c2p == -1) return 1;
	if(read(c2p, &sum, sizeof(int)) == -1) return 2;	
	//Print out array numbers and free allocated memory space
	for(int i=0; i<num; i++){ 
	printf("Array [%d]->%d\n", i, *(array+i));
	}
	free(array);
	printf("Total sum is %d\n", sum);
	wait(0);
	remove("c2p");
	remove("p2c");	
}
