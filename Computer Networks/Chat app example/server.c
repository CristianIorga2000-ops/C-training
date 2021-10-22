#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 255 
#define DEFAULT_PORT 1234
#define MAX_CLIENTS 1 //max number of clients that can connect to the sever
//change MAX_CLIENTS if you want to make a group chat, for example


void error(const char *msg){
	perror(msg);
	exit(1);
}

//If an integer is given as the firs command line argument, port is changed to that integer
void setPort(int argc, char* argv[], int *port){
	if(argc > 1){
		sscanf(argv[1], "%d", port);
		printf("Analyzing command line arguments:\n");
	}
	if (*port != DEFAULT_PORT)
		printf("Port takes value: %d.\n", *port);
	else
		printf("Port remained to default value %d.\n", DEFAULT_PORT); 
}

int main(int argc, char* argv[]){
	int port = DEFAULT_PORT;
	setPort(argc, argv, &port);
	int
	sockfd, // socket file descriptor
	newsockfd; // new socket file descriptor
	char buffer[BUFFER_SIZE]; // we will store our messages here;
	struct sockaddr_in server_address, client_address; // internet addresses
	socklen_t client_length;
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0 ){
		error("Error opening Socket");
	}
	bzero((char *) &server_address, sizeof(server_address));
	
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(port);
	
	if(bind(sockfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
		error("Socket binding failed");
	
	listen(sockfd, MAX_CLIENTS);
	client_length = sizeof(client_address);
	
	newsockfd = accept(sockfd, (struct sockaddr *) &client_address, &client_length);
	
	if(newsockfd < 0)
		error("Error on accept()");
	
	while(1){
		bzero(buffer, BUFFER_SIZE); //empty the buffern
		if(read(newsockfd, buffer, BUFFER_SIZE) <0)
			error("Error on reading");
		printf("#CLIENT:--  %s\n", buffer);
		bzero(buffer, BUFFER_SIZE);
		fgets(buffer, BUFFER_SIZE, stdin);
		if(write(newsockfd, buffer, BUFFER_SIZE) < 0)
			error("Error on writing");
	}
	close(newsockfd);
	close(sockfd);
	return 0;
}