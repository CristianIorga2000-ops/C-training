#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
//#include <semaphore.h>

#define DICE_MAX 6
#define SLEEP_MAX 3
#define NR_THREADS 10

int throws[NR_THREADS];
int isWinner[NR_THREADS] = { [0 ... NR_THREADS - 1] = 0};

pthread_barrier_t waitForDiceThrow;
int waitForNomination = 1;

//Thread routine
//Threads will throw dice
//They will wait for winner nomination
//They will yell at you wether they won or lost
void* throwDice(void* arg){
	int id = *(int*)arg;
	int dice = 1 + rand() % DICE_MAX;
	throws[id-1] = dice;
	sleep(1 + rand() % SLEEP_MAX);
	printf("Thread %d: I threw %d!\n", id, dice);
	pthread_barrier_wait(&waitForDiceThrow);
	while(waitForNomination) {};
	if(isWinner[id-1]) 
		printf("Thread %d: I WON! I FRICKIN` WON!!!\n", id);
	else
		printf("Thread %d: I LOST! GOD`DINGIT!\n", id);
	free(arg);
	return NULL;
}

int main(int argc, char* argv[]){
	int n = NR_THREADS;
	pthread_barrier_init(&waitForDiceThrow, NULL, 1 + n); 
	//sem_init(&waitForNomination, 0, 0);
	//Create threads
	pthread_t threads[n];
	for(int i=0; i<n; i++){
		int *id = malloc(sizeof(int));
		*id = i+1;
		if(pthread_create(threads+i, NULL, &throwDice, (void*) id) != 0)
			{perror("Thread create error"); exit(0);}
	}
	printf("All threads created!\n");
	
	pthread_barrier_wait(&waitForDiceThrow);
	printf("All threads threw the dice!\n");

	int max=-1;
	//Calc Max value
	for(int i=0; i<n; i++)
		if(max < throws[i]) max = throws[i];
	
	//Nominate winners
	for(int i=0; i<n; i++)
		if(throws[i] == max) isWinner[i] = 1;
	waitForNomination = 0;
	//Thread join
	for(int i=0; i<n; i++){
		if(pthread_join(threads[i], NULL)!=0)
			{perror("Thread join error"); exit(0);}
	}

	pthread_barrier_destroy(&waitForDiceThrow);
	return 0;
}









