#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<time.h>

void* roll(){
	int *res;
	res = malloc(sizeof(int));
	*res = 1 + (rand()%6);
	return (void*) res;
}

int main(int argc, char* argv[]){
	srand(time(NULL));
	int n, *res;
	if(argc == 2) sscanf (argv[1],"%d",&n);
	else {
		printf("Read n (number of threads) -> \n");
		scanf("%d", &n);
	}
	printf("You read %d.\n", n);
	pthread_t t[n];
	for(int i=0; i<n; i++) 
		if(pthread_create(t+i, NULL, &roll, NULL) != 0)
			perror("Failed create");
	for(int i=0; i<n; i++){
		if(pthread_join(*(t+i), (void**) &res) != 0) perror("Failed join");
		printf("Thread %d threw: %d!\n", i+1, *res);
		free(res);
	}
	return 0;
}
