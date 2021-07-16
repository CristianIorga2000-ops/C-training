#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

//A structure containing two integers, a prime number and an id
typedef struct{
	int id;
	int primeNumber;
}threadArg;


//Function that calculates the first n prime numbers and returns a pointer to an array
//containing them in descending order
int* primes(int n){
	int * array;
	array = calloc(n, sizeof(int));
	for(int i=2; n>0; i+=1){
		int prime = 1;
		for(int j=2; j<=i/2; j++) 
			if (i%j == 0){prime = 0; break;}
		if(prime) *(array + --n) = i;	
	}
	return array;
}

//Routine for threads: print thread id and associated prime number
//Thread id represents the number of the thread in thread creation
//(The second created thread has id 2)
void* routine(void* arg){
	threadArg *thread = (threadArg*)arg;
	int id = thread->id;
	int prime = thread->primeNumber;
	printf("Thread %i has prime number: %d.\n", id, prime);
	free(arg);
	return NULL;
}

int main(int argc, char* argv[]){
	int n;
	if(argc == 2) sscanf(argv[1], "%d", &n);
	else {
		printf("Read n(number of threads) -> \n");
		scanf("%d", &n);
	}
	pthread_t threads[n];
	int* primeArray = primes(n);
	for(int i=0; i<n; i++){
		threadArg* a;
		a = malloc(sizeof(threadArg));
		a->id = i+1;
		a->primeNumber = *(primeArray+n-i);
		if(pthread_create(threads+i, NULL, &routine, (void*) a) != 0)
			perror("Failed thread create");
	}
	printf("Oh boy, I finally finished creating all these threads.\n");
	for(int i=0; i<n; i++){
		if(pthread_join(*(threads+i), NULL) != 0)
			perror("Failed thread join");
	}
	free(primeArray);
	return 0;
}
