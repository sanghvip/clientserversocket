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
int getRandomInteger(int n){
  srand(time(NULL));
  return(rand()%n + 1);
}

void serviceClient(int,int);

int main(int argc, char *argv[]){
	int sock, connClient, port,remoteConn,turn;
	int scoreServer=0,scoreServerTotal=0,scoreClient=0,scoreClientTotal=0,status;
	struct sockaddr_in servConfig;
	char message[255];
	
	
	if(argc != 2){
		printf("Call model: %s <Port Number>\n", argv[0]);
    exit(0);
	}
	
	//create a soccket for connection
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    fprintf(stderr, "Cannot create socket\n");
    exit(1);
     }
	 sock = socket(AF_INET, SOCK_STREAM, 0);
	 servConfig.sin_family = AF_INET;
	 servConfig.sin_addr.s_addr = htonl(INADDR_ANY);
	 sscanf(argv[1],"%d",&port);
	 printf("Port number:%d\n",port);
	 servConfig.sin_port = htons((uint16_t)port);
	 
	 bind(sock,(struct sockaddr *)&servConfig,sizeof(servConfig));	 
	 listen(sock,5);
	 
	 while(1)
	 {
		 printf("Waiting for client..\n");
		 
		 //accept connection
		 remoteConn = accept(sock,NULL,NULL);
		 printf("Got client\n");
		 
		 //send directive message to client
		 send(remoteConn,"Hello Client\n",sizeof("Hello Client\n"),0);
		 
		 if(!fork())
			 serviceClient(remoteConn,0);
		 close(remoteConn);
		 waitpid(0, &status, WNOHANG);
		 //exit(0);
	}	 
}

void serviceClient(int remoteConn,int pid){
	int scoreServer=0,dice=0,n=10;
	char str[6];
	int data[5]={0,0,0,0,3};
	while(1){
	if(data[4]!=0){
    if(data[2]>=100){
		printf("Game Over:Server won the game\n");
		printf("Server:Server total=%d\n",data[2]);
		printf("Server:Client total= %d\n",data[3]);
		data[4]=1;
		send(remoteConn,&data,sizeof(data),0);
		close(remoteConn);
		exit(0);
		//kill(0,SIGTERM);	
	}
	else if(data[3]>=100){
		printf("Game Over:Client won the game\n");
		printf("Server:Server total=%d\n",data[2]);
		printf("Server:Client total= %d\n",data[3]);
		data[4]=2;
		send(remoteConn,&data,sizeof(data),0);
		close(remoteConn);
		exit(0);
		//kill(0,SIGTERM);	
	}
	else{
		printf("Game on\n");
	}
	printf("*****************************************\n");
	printf("Refree:Server playing with client %d...\n",data[0]);
	printf("*****************************************\n");
	printf("Press enter to play or Bye to exit\n");	
	gets(str);
	if(strcmp(str,"Bye")==0){
			//exit(1);
			data[4]=0;
			send(remoteConn,&data,sizeof(data),0);
			//break;
			exit(0);
			kill(0,SIGTERM);					
		}
	printf("*****************************************\n");
	dice=getRandomInteger(n);
	//increase server score
	//data[0] = data[0]+dice;
	//increase the server total
	data[2] = data[2]+dice;
	printf("Server:Dice=%d\n",dice);
	printf("Server:Server total=%d\n",data[2]);
	printf("Server:Client total= %d\n",data[3]);
	printf("*****************************************\n");
	printf("Refree:Client %d playing \n\n",data[0]);
	printf("*****************************************\n");
	
	send(remoteConn,&data,sizeof(data),0);
	
	recv(remoteConn,&data,sizeof(data),0);
	}
	else{
		printf("Client Exited\n");
		exit(0);
		kill(0,SIGTERM);			
		
	}
	}
}
