//client
#include <time.h>
#include <string.h>
#include<sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>

int getRandomInteger(int n){
  return(rand()%n + 1);
}

int main(int argc, char *argv[]){
	srand(time(NULL));
	int sock, port,dice=0;
	char message[255];
	//str array to record string inputs and messages
	char str[6];
	//Fields in data array
	//First element tracks the server dice value
	//Second element tracks the client dice value
	//Third element tracks the server score
	//Fourth element tracks the client score
	//Fifth element specifies the number of turns
	int data[5],n=10,id=0;
	struct sockaddr_in clientConfig;
	
	if(argc != 3){
		printf("Command Signature: %s IPAddress PortNumber\n", argv[0]);
    exit(0);
	}
	//create a soccket for connection
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    fprintf(stderr, "Socket cannot be created\n");
    exit(1);
     }
	 clientConfig.sin_family = AF_INET;
	 sscanf(argv[2],"%d",&port);
	 clientConfig.sin_port = htons((uint16_t)port);
	 
	 if(inet_pton(AF_INET,argv[1],&clientConfig.sin_addr)<0){
		   fprintf(stderr, " Conversion of internet address has failed\n");
			exit(2);
	 } 
	 
	 //connect to server
	  if(connect(sock, (struct sockaddr *) &clientConfig, sizeof(clientConfig))<0){
		fprintf(stderr, "connect() has failed, exiting\n");
		exit(3);
		}
		
		printf("Connected to server\n");
		
		//read message from server
		if((read(sock,&message,sizeof(message)))<0){
			printf("No directive message from server\n");
			exit(0);
		}
		printf("%s\n",message);
		
			while(1){
				//check if client has sent data
			if((read(sock,&data,sizeof(data)))>0){
				if(data[4]>2){
				printf("Game on: you can now play your dice\n");
				printf("*****************************************\n");
				printf("Client playing ..\n");
				printf("*****************************************\n");
				printf("Press enter to continue or Bye to exit\n");
				gets(str);
				
				if(strcmp(str,"Bye")==0){
					data[4]=0;
					write(sock,&data,sizeof(data));
					exit(0);
					}
				
				printf("*****************************************\n");
				dice = getRandomInteger(n);
				//increase client score
				data[3] = data[3]+dice;
				printf("Client:Dice=%d\n",dice);
				printf("Client:Client Total=%d\n",data[3]);
				printf("Client:Server Total= %d\n",data[2]);				
				printf("*****************************************\n");
				printf("Server Playing....\n");
				printf("*****************************************\n");
				
				//write data to client
				write(sock,&data,sizeof(data));
				}
				//check if server exited
				else if(data[4]==0)
				{
						printf("Server Exited\n");			
						exit(0);
				}
				//check if server won the game
				else if(data[4]==1){
					printf("Game over:Server won the game\n");
					printf("Client:Client Total=%d\n",data[3]);
					printf("Client:Server Total= %d\n",data[2]);	
                    printf("*****************************************\n");
					exit(0);
				}
				//check if client won the game
				else if(data[4]==2){
					printf("Game over:Client won the game\n");
					printf("Client:Client Total=%d\n",data[3]);
					printf("Client:Server Total= %d\n",data[2]);	
					printf("*****************************************\n");					
					exit(0);
				}
					
				}				
			}
}
