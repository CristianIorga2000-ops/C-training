#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NR_THREADS 18
#define SERVER_LIMIT 5

sem_t serverLimit;

void* routine(void* arg){
	int id = *(int*)arg;
	sem_wait(&serverLimit);
	printf("LOGIN:  %d\n", id);
	sleep(2+rand()%4);
	printf("LOGOUT: %d\n", id);
	sem_post(&serverLimit);
	free(arg);
	return NULL;	
}

int main(){
	int n = NR_THREADS;
	sem_init(&serverLimit, 0, SERVER_LIMIT);
	pthread_t threads[n];
	for(int i=0; i<n; i++){
		int *id = malloc(sizeof(int));
		*id = i+1;
		if(pthread_create(threads+i, NULL, &routine, (void*) id) != 0)
			{perror("Thread create error"); exit(0);}
	}

	for(int i=0; i<n; i++){
		if(pthread_join(threads[i], NULL) != 0)
			{perror("Thread join error"); exit(0);}
	}
	sem_destroy(&serverLimit);
	return 0;
}
