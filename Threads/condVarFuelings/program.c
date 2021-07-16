#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h> // for sleep in refiller threads to create the illusion of life that will fill the empty voids in your heart
#include <time.h> // for seeding random

#define SLEEP_TIME 3 //How much time a fuel replenishment will last
#define REFILL_COUNT 5 //number of refills that producer threads will do
int choices[] = {15, 30, 45, 20, 50, 60}; // possible values for consumer threads amount of consumed fuel

pthread_mutex_t lockFuel;
pthread_cond_t condFuel;

int fuel = 0;

//Return a random integer from choices integer vector
int getConsumption(){
	int choiceCount = sizeof(choices)/ sizeof(int);
	return choices[rand() % choiceCount];
}

//Process arguments
//Request input for N or M if argument is missing
//Correct N and M to valid values if needed
void processArgs(int *n, int *m, int argc, char* argv[]){
	*n = -1000; *m = -1000;
	//Note that argc == 1 means we have no arguments,
	//	only the function call itself
	if(argc>1)//If we have at least one argument:
		//Convert first argument to integer
		// and assign it to n
		sscanf(argv[1], "%d", n);
	if(argc>2)//If we have at least two arguments:
		//Convert second argument to integer
		// and assign it to m
		sscanf(argv[2], "%d", m);
	if(*n == -1000){ //if n is unchanged
		printf("Give N (number of consumer threads) -> ");
		scanf("%d", n);
	}
	if(*m == -1000){ //if m is unchanged
		printf("Give M (number of producer threads) -> ");
		scanf("%d", m);
	}
	if(*n<=0){
		*n = -*n + 1;
		printf("N was correced to a valid value.\n");
	}
	if(*m<=0){
		*m = -*m + 1;
		printf("M was corrected to a valid value.\n");
	}

}

//structure created for being passed as argument to thread routines
typedef struct{
	int id;
	int consumption;
} threadArg;

//Routine for consumer threads
//Thread will wait until it can consume
//Thread will prompt if it cannot consume
//Thread will prompt if it consumed
void* consumeFuel(void* arg){
	threadArg* currentArg = (threadArg*) arg;
	int id = currentArg->id;
	int consumption = currentArg->consumption;
	pthread_mutex_lock(&lockFuel);
	while( fuel < consumption){
		printf("COMPLAINT#     Consumer %d: I cannot consume fuel!\n", id);
		pthread_cond_wait(&condFuel, &lockFuel);
	}
	fuel -= consumption;
	printf("CONSUMPTION#   Consumer %d: I drank %d gallons of gasoline!\n", id, consumption);
	pthread_mutex_unlock(&lockFuel);
	free(arg);
	return NULL;
}

//Routine for refiller threads
//Thread will refill REFILL_COUNT times a precalculated amount of fuel
//Thread will broadcast after each fuel
void* refillFuel(void* arg){
	threadArg* currentArg = (threadArg*) arg;
	int id = currentArg->id;
	int consumption = currentArg->consumption;
	for(int i=0; i<REFILL_COUNT; i++){
		pthread_mutex_lock(&lockFuel);
		fuel += consumption;
		printf("REFILL#        Refiller %d: I refilled. Now pool has %d gallons.\n", id, fuel);
		pthread_mutex_unlock(&lockFuel);
		pthread_cond_broadcast(&condFuel);
		sleep(SLEEP_TIME);
	}
	free(arg);
	printf("Refiller %d: I have finished my job. Now i can go watch One Piece.\n", id);
	return NULL;
}

int main(int argc, char* argv[]){
	srand(time(NULL));
	int n, m;
	int totalConsumption=0;
	processArgs(&n, &m, argc, argv);
	printf("N final value is %d\nM final value is %d\n\n", n, m);
	pthread_t threads[n+m];
	pthread_mutex_init(&lockFuel, NULL);
	pthread_cond_init(&condFuel, NULL);
	for(int i=0; i<m+n; i++){
		//If we are creating a consumer thread
		if(i<n){
			threadArg *currentArg;
			currentArg = malloc(sizeof(threadArg));
			currentArg->id = i+1;
			currentArg->consumption = getConsumption();
			totalConsumption += currentArg->consumption;
			if(pthread_create(threads+i, NULL, &consumeFuel, currentArg) != 0)
			{perror("Failed thread create"); exit(0);}
		}
		//If we are creating a refiller thread
		else{

			threadArg *currentArg;
			currentArg = malloc(sizeof(threadArg));
			currentArg->id = i-n+1;
			currentArg->consumption = totalConsumption/ (m * REFILL_COUNT);
			if(pthread_create(threads+i, NULL, &refillFuel, currentArg) != 0)
				{perror("Failed thread create"); exit(0);}

		}
	}
	printf("\n\nMAIN#          Master, I have finished creating the world...\n\n\n");
	for(int i=0; i<m+n; i++){
		if(pthread_join(threads[i], NULL) != 0)
			{perror("Failed thread join"); exit(0);}
	}
	pthread_mutex_destroy(&lockFuel);
	pthread_cond_destroy(&condFuel);
	return 0;
}









