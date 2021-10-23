#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>

#define BUFFER_SIZE 255 
#define DEFAULT_PORT 1234

void error(const char *msg){
	perror(msg);
	exit(1);
}

void parseArgs(int argc, char *argv[],int *port){
	if (argc > 2)
		sscanf(argv[2],"%d", port);
	else if (argc < 2)
		perror("You must give the ip and (optionally) the port as command line arguments");
	printf("Selected port is: %d\n", *port);
}

int main(int argc, char *argv[]){
	int port = DEFAULT_PORT;
	parseArgs(argc, argv,&port);
	
	char *ip = argv[1];
	printf("Selected ip is: %s\n", ip);	
	char buffer[BUFFER_SIZE];
		
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_address;
	
	struct hostent *server;
	server = gethostbyname(argv[1]);
	if (server == NULL)
		error("Such a host does not exist");

	
	bzero((char *) &server_address, sizeof(server_address));
	server_address.sin_family = AF_INET;
	bcopy((char *) server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length);
	server_address.sin_port = htons(port);
	
	if(connect(sockfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
		error("Connection failed");
	
	while(1){
		bzero(buffer, BUFFER_SIZE);
		fgets(buffer, BUFFER_SIZE, stdin);
		if(write(sockfd, buffer, BUFFER_SIZE) < 0)
			error("Error on writing");
				bzero(buffer, BUFFER_SIZE); //empty the buffer
		if(read(sockfd, buffer, BUFFER_SIZE) <0)
			error("Error on reading");
		printf("#SERVER:--  %s\n", buffer);
	}
	close(sockfd);
}
