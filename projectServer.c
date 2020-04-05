//server
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include<sys/wait.h>
#include <signal.h>
#include <time.h>
#include <string.h>

static int clientNo=0;
int getRandomInteger(int n){  
  return(rand()%n + 1);
}

void serviceClient(int,int);

int main(int argc, char *argv[]){
	srand(time(NULL));
	int sock,port,remoteConn;
	int status;
	struct sockaddr_in servConfig;
	char message[255];	
	
	if(argc != 2){
		printf("Command signature: %s Port Number\n", argv[0]);
    exit(0);
	}
	
	//create a soccket for connection
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    fprintf(stderr, "Socket cannot be created\n");
    exit(1);
     }
	 sock = socket(AF_INET, SOCK_STREAM, 0);
	 servConfig.sin_family = AF_INET;
	 servConfig.sin_addr.s_addr = htonl(INADDR_ANY);
	 sscanf(argv[1],"%d",&port);
	 servConfig.sin_port = htons((uint16_t)port);
	 
	 //bind the socket
	 bind(sock,(struct sockaddr *)&servConfig,sizeof(servConfig));	 
	 
	 //listen to new connections
	 listen(sock,3);
	 
	 while(1)
	 {
		 printf("Waiting for client..\n");
		 
		 //accept connection
		 remoteConn = accept(sock,NULL,NULL);
		 
		 //keep track of number of clients
		 clientNo++;
		 
		 //manage the client in new child
		 if(!fork())
			 serviceClient(remoteConn,clientNo);
		close(remoteConn);
		waitpid(0,&status,WNOHANG);
	}	 
}

void serviceClient(int remoteConn,int clientNo){
	printf("Got client\n");		 
	
	//send directive message to client
	write(remoteConn,"Hello Client\n",sizeof("Hello Client\n"));
	int dice=0,n=10;
	char str[6];
	//Fields in data array
	//First element tracks the server dice value
	//Second element tracks the client dice value
	//Third element tracks the server score
	//Fourth element tracks the client score
	//Fifth element specifies the number of turns
	int data[5]={0,0,0,0,3};
	while(1){
	if(data[4]!=0){
		
	//check if server has won	
    if(data[2]>=100){
		printf("Game Over:Server won the game\n");
		printf("Server:Server total=%d\n",data[2]);
		printf("Server:Client total= %d\n",data[3]);
		printf("*****************************************\n");	
		data[4]=1;
		write(remoteConn,&data,sizeof(data));
		exit(0);
	}
	
	//check if client has won
	else if(data[3]>=100){
		printf("Game Over:Client won the game\n");
		printf("Server:Server total=%d\n",data[2]);
		printf("Server:Client total= %d\n",data[3]);
		data[4]=2;
		write(remoteConn,&data,sizeof(data));
		exit(0);
	}
	else{
		printf("Game on\n");
	}
	
	printf("*****************************************\n");
	printf("Refree:Server playing with client %d...\n",clientNo);
	printf("*****************************************\n");
	printf("Press enter to play or Bye to exit\n");	
	gets(str);
	if(strcmp(str,"Bye")==0){
			data[4]=0;
			write(remoteConn,&data,sizeof(data));
			exit(0);					
		}
	dice=getRandomInteger(n);
	
	//add dice value to the server score
	data[2] = data[2]+dice;
	printf("*****************************************\n");	
	printf("Server:Dice=%d\n",dice);
	printf("Server:Server total=%d\n",data[2]);
	printf("Server:Client total= %d\n",data[3]);
	printf("*****************************************\n");
	printf("Refree:Client %d playing \n\n",clientNo);
	printf("*****************************************\n");
	
	write(remoteConn,&data,sizeof(data));		
	read(remoteConn,&data,sizeof(data));
	}
	else{
		printf("Client %d Exited\n",clientNo);
		exit(0);			
		
	}
	}
}
