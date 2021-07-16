#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>


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

typedef struct{
	int id;
	int leap;
	int* array;
}threadArg;

//Routine for threads: calculate own part of array
//arg is composed from: thread id, thread count/ array size
void* routine(void* arg){
	int *sum;
	sum = malloc(sizeof(int));
	*sum = 0;
	//Cast from void* to threadArg* so we can acces members cleanly
	threadArg *thread = (threadArg*) arg;
	//Take id and leap
	int id = thread->id;
	int leap = thread->leap;
	//Calculate start and end positions in array
	int start = id*leap;
	int end = (id+1)*leap-1;
	printf("ID: %d\n Leap: %d\nStart: %d\nEnd: %d\n", id, leap, start, end);
	printf("Thread %d awakens... it goes %d->%d in the array..\n",id, start, end);
	//Calculate sum
	int* array = thread->array;
	for(int i=start; i<=end; i++)
	{*sum += *(array+i); 
		printf("Thread %d: I added %d!\n", id, *(array+i));}
	free(arg);
	return (void*)sum;	
}

int main(int argc, char* argv[]){
	int *r; //Here we hold the return values 
	int sum=0;
	int n=-1; //Number of threads
	int arraySize=-1; 
	//If we have at least one argument, that argument may be n
	//Convert it to int
	//Note that the command to run the program is an argument in itself
	//So we need argc to be at least 2 
	if(argc>1) sscanf(argv[1], "%d", &n);
	//Same if we have 2 arguments
	if(argc>2) sscanf(argv[2], "%d", &arraySize);
	//If these values were not passed as arguments, read them from input
	if(n == -1) {
		printf("Please give number of threads-> ");
		scanf("%d", &n);
	}
	if(arraySize == -1){
		printf("Please give array size-> ");
		scanf("%d", &arraySize);
	}
	//N must be positive
	//We must have ArraySize % n == 0
	if(n<0) {
		printf("N was corrected to be a positive value\n");
		n = -n;
	}
	if(arraySize %  n != 0){
		printf("ArraySize was corrected to be divisible by N\n");
		arraySize += n - arraySize%n;
	}
	printf("Final N (number of threads) is: %d\n", n);
	printf("Final arraySize is: %d\n", arraySize);
	//Now we can start multithreading!
	pthread_t threads[n];
	int *array = primes(arraySize);
	printf("Array: {");
	for(int i=0; i<arraySize; i++) printf("%d, ", *(array+i));
	printf("}\n");
	for(int i=0; i<n; i++){
		threadArg *a;
		a = malloc(sizeof(threadArg));
		a->leap = arraySize/n;
		a->id = i;
		a->array = array;
		if (pthread_create(threads+i, NULL, &routine, (void*) a) != 0)
		{perror("Failed thread create"); exit(0);}
	}
	printf("I have finished thread spawning, master...\n");
	for(int i=0; i<n; i++){
		if(pthread_join(threads[i], (void**)&r) != 0)
		{perror("Failed thread join"); exit(0);}
		printf("Thread %d says: Master, I have sum %d!\n", i, *r);
		sum += *r;
		free(r);
	}
	printf("Master, final sum is %d! Be praised!\n", sum);
	return 0;
	free(array);	
}
