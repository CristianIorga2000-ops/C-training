#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
	int arr[]={1, 2, 10, 2, 2, 2, 6, 6, 6};
	int arraySize = sizeof(arr)/ sizeof(int);
	int start, end;
	int  id1, id2;
	int sum=0;
	int com1[2], com2[2];
	if(pipe(com1) == -1) return 3;
	if(pipe(com2) == -1) return 4;
	id1 = fork();
	if(id1 == -1) return 1;
	if(id1 != 0){//in parent
		id2 = fork();
		if(id2 == -1) return 2;
		start = 0;
		end = arraySize/3; 	
	}
	if(id1 == 0){//third child
		start = arraySize/3;
		end = arraySize/3*2;
	}
	if(id2 == 0){//second child
		start = arraySize/3*2;
		end=arraySize;
	}
	for(int i=start; i<end; i++) sum+=arr[i];
		printf("My sum is: %d\n", sum);		
	if(id1 == 0){//child1 communicates with parent trough pipe1  
		close(com2[1]);
		close(com2[0]);
		close(com1[0]);
		if(write(com1[1], &sum, sizeof(sum)) == -1) return 8;
		close(com1[1]);
		exit(0);
	}

        if(id2 == 0){//child2 communicates with parent trough pipe2
         	close(com1[0]);
		close(com1[1]);
	        close(com2[0]);
                if(write(com2[1], &sum, sizeof(sum)) == -1) return 7;
		close(com2[1]);
        	exit(0);
	}
	//Only parent here
	close(com1[1]);
	close(com2[1]);
	int sumFromChild1, sumFromChild2;
	if(read(com1[0], &sumFromChild1, sizeof(int)) == -1) return 5;
	if(read(com2[0], &sumFromChild2, sizeof(int)) == -1) return 6;
	printf("sumFromChild1 is: %d\nsumFromChild2 is: %d\n", sumFromChild1, sumFromChild2);
	
	int finalSum = sum + sumFromChild1 + sumFromChild2;
	printf("Final sum is: %d\n", finalSum);
	return 0;

}
