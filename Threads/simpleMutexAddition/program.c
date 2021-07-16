#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int count=0;
pthread_mutex_t lock;
void* routine(){
	for(int j=0; j<1000000; j++){
		pthread_mutex_lock(&lock);
		count++;
		pthread_mutex_unlock(&lock);
	}
	return NULL;
}
int main(int argc, char* argv[]){
	int n;
	if(argc > 1) sscanf(argv[1], "%d", &n);
	else{
		printf("Give n-> ");
		scanf("%d", &n);
	}
	pthread_t threads[n];
	pthread_mutex_init(&lock, NULL);
	for(int i=0; i<n; i++){
		if(pthread_create(threads+i, NULL, &routine, NULL) != 0)
			perror("Failed thread create"); 
	}

	for(int i=0; i<n; i++){
		if(pthread_join(*(threads+i), NULL) != 0)
			perror("Failed thread join");
	}
	printf("Count is now: %d\n", count);
	pthread_mutex_destroy(&lock);
	return 0;
}
