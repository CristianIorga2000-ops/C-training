#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 10

sem_t consumerSem, producerSem;
pthread_mutex_t lock;

int buffer[BUFFER_SIZE];
int currentPos = 0;


void* consume(void* arg){
	while(1){
		sem_wait(&consumerSem);

		pthread_mutex_lock(&lock);
		
		printf("CONSUMER: %d\n", buffer[currentPos]);
		currentPos--;
		
		pthread_mutex_unlock(&lock);

		sem_post(&producerSem);
	}
}

void* produce(void* arg){
	while(1){
		int product = 1 + rand()%100;
		sleep(1);
		sem_wait(&producerSem);

		pthread_mutex_lock(&lock);
	
		printf("PRODUCER: %d\n", product);
		buffer[++currentPos] = product;

		pthread_mutex_unlock(&lock);
		
		sem_post(&consumerSem);
		
	}
}

void processArgs(int *n, int *m, int argc, char* argv[]){
        *n = -1000; *m = -1000;
        if(argc>1)
                sscanf(argv[1], "%d", n);
        if(argc>2)
                sscanf(argv[2], "%d", m);
        if(*n == -1000){
                printf("Give N (number of consumer threads) -> ");
                scanf("%d", n);
        }
        if(*m == -1000){
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

int main(int argc, char* argv[]){
	int n, m;
	processArgs(&n, &m, argc, argv);

	pthread_mutex_init(&lock, NULL);

	sem_init(&producerSem, 0, BUFFER_SIZE);
	sem_init(&consumerSem, 0, 0);


	pthread_t threads[n];
	for(int i=0; i<n+m; i++){
		if(i<n){
			if(pthread_create(threads+i, NULL, &consume, NULL) != 0)
				{perror("Error create thread"); exit(1);}
		}
		else{
			if(pthread_create(threads+i, NULL, &produce, NULL) != 0)
				{perror("Error create thread"); exit(1);}
		}
	}


	for(int i=0; i<n+m; i++){
		if(pthread_join(threads[i], NULL) != 0)
			{perror("Error join thread"); exit(1);}
	}

	sem_destroy(&producerSem);
	sem_destroy(&consumerSem);

	pthread_mutex_destroy(&lock);
	return 0;
}
