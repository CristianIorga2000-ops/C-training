#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<time.h>

#define MAX_CONSUMPTION 30 // Maximum amount of fuel a chef can consume
#define STOVE_CAPACITY 100 //How much fuel a stove has
#define NR_STOVES 5 // Number of available stoves

pthread_mutex_t lockFullStoves; //Lock acces to fullStoves variable
pthread_mutex_t locks[NR_STOVES]; //Lock acces to each stove
int stoves[NR_STOVES] = {[0 ... NR_STOVES-1] = 100}; // !!! GNU Syntax !!!
int fullStoves = NR_STOVES; //Number of full stoves

//Get a random number between 1 and MAX_CONSUMPTION
int getConsumption(){
	return rand() % MAX_CONSUMPTION + 1;
}

//Process arguments
//Request input for N if argument is missing
//Correct N to valid value if needed
void processArgs(int *n, int argc, char* argv[]){
	*n = -1000;
	if(argc>1)
		sscanf(argv[1], "%d", n);
	if(*n == -1000){ 
		printf("Give N (number of enslaved chefs) -> ");
		scanf("%d", n);
	}
	if(*n<=0){
		*n = -*n + 1;
		printf("N was correced to a valid value.\n");
	}
}

//Routine for chef threads
//Chefs will keep looking for an empty stove to cook on
//If all stoves are out of gas, they are free to leave the kitchen
//And go home to the family that has been waiting for them for months...
void* cook(void* arg){
	int id = *(int*)arg;
	int m = NR_STOVES;
	while(fullStoves > 0){
		for(int i=0; i<m; i++){
			if(pthread_mutex_trylock(locks+i) == 0){
				//This chef has succesfully occupied a stove and is using it
				sleep(1 + rand()%2);
				int consumption = getConsumption();
				if(stoves[i]>0){
					//If this is the last cooking that will be done on this stove
					if(stoves[i] - consumption <= 0 ){
						consumption = stoves[i]; // This is to avoid consuming more fuel than available
						pthread_mutex_lock(&lockFullStoves);
						fullStoves--;
						printf("FS: %d\n", fullStoves);
						pthread_mutex_unlock(&lockFullStoves);
					}
					stoves[i] -= consumption;
					printf("Chef %d cooked on stove %d, using %d fuel. Stove %d now has %d fuel.\n", id, i+1, consumption, i+1, stoves[i]);
				}
				pthread_mutex_unlock(locks+i);
			}
		}
	}
	printf("Chef %d finished his work. With teary eyes and torn apart clothes, he goes home...\n", id);
	free(arg);
	return NULL;
}

int main(int argc, char* argv[]){
	int m, n;
	m = NR_STOVES;
	processArgs(&n, argc, argv);
	printf("Chefs: %d\nStoves: %d\n", n, m); 

	//Create mutex locks for each stove
	pthread_mutex_t locks[m];
	for(int i=0; i<m; i++)
		pthread_mutex_init(locks+i, NULL);
	pthread_mutex_init(&lockFullStoves, NULL);
	printf("ARCHITECT#     The chains have been forged. We are ready to chain the slaves.\n");

	//Create threads
	pthread_t threads[n];
	for(int i=0; i<n; i++){
		int *id = malloc(sizeof(int));
		*id = i+1;
		if(pthread_create(threads+i, NULL, &cook, (void*) id)!=0)
		{perror("Thread create fail"); exit(0);}
	}
	printf("ARHITECT#      The slaves have been put to work.\n");

	//Wait for threads
	for(int i=0; i<n; i++){
		if(pthread_join(threads[i], NULL)!=0)
		{perror("Thread join fail"); exit(0);}
	}
	printf("ARCHITECT#    All slaves have finished their work. They may return to their old lives. For now..\n");

	for(int i=0; i<n; i++){
		pthread_mutex_destroy(locks+i);
	}
	pthread_mutex_destroy(&lockFullStoves);
	return 0;
}













