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
  srand(time(NULL));
  return(rand()%n + 1);
}

int main(int argc, char *argv[]){
	int sock, port,dice=0;
	char message[255];
	char str[6],ipaddr[20];
	int data[5],n=10,id=0;
	struct sockaddr_in clientConfig;
	
	if(argc != 3){
		printf("Call model: %s <IP Address> <Port Number>\n", argv[0]);
    exit(0);
	}
	
	//create a soccket for connection
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    fprintf(stderr, "Cannot create socket\n");
    exit(1);
     }
	 clientConfig.sin_family = AF_INET;
	 printf("Enter IP address of server:");
	 gets(ipaddr);
	 printf("Enter Port number:");
	 scanf("%d",&port);
	 //sscanf(argv[2],"%d",&port);
	 clientConfig.sin_port = htons((uint16_t)port);
	 
	 if(inet_pton(AF_INET,ipaddr,&clientConfig.sin_addr)<0){
		   fprintf(stderr, " inet_pton() has failed\n");
			exit(2);
	 }
	 printf("Port Number:%d\n",port);
	 printf("Enter unique client number\n");
	 scanf("%d",&id);
	 printf("Data:%d\n",id);
	 
	 //connect to client
	  if(connect(sock, (struct sockaddr *) &clientConfig, sizeof(clientConfig))<0){
		fprintf(stderr, "connect() has failed, exiting\n");
		exit(3);
		}
		
		printf("Connected to server\n");
		
		if((recv(sock,&message,sizeof(message),0))<0){
			printf("No message received\n");
			exit(0);
		}
		printf("%s\n",message);
		//if parent then read servers mesage
			while(1){
			if((recv(sock,&data,sizeof(data),0))>0){
				if(data[4]>2){
				printf("Game on: you can now play your dice\n");
				printf("*****************************************\n");
				printf("Client %d playing ..\n",id);
				printf("*****************************************\n");
				printf("Press enter to continue or Bye to exit\n");
				gets(str);
				if(strcmp(str,"Bye")==0){
					data[4]=0;
					send(sock,&data,sizeof(data),0);
					//kill(pid,SIGTERM);					
					exit(0);
					}
				
				printf("*****************************************\n");
				dice = getRandomInteger(n);
				//increase client score
				data[0]=id;
				//increase the client total
				data[3] = data[3]+dice;
				printf("Client:Dice=%d\n",dice);
				printf("Client:Client Total=%d\n",data[3]);
				printf("Client:Server Total= %d\n",data[2]);				
				printf("*****************************************\n");
				printf("Server Playing....\n");
				printf("*****************************************\n");
				
				send(sock,&data,sizeof(data),0);
				}
				else if(data[4]==0)
				{
						printf("Server Exited\n");
						//kill(pid,SIGTERM);					
						exit(0);
				}
				else if(data[4]==1){
					printf("Game over:Server won the game\n");
					printf("Client:Client Total=%d\n",data[3]);
					printf("Client:Server Total= %d\n",data[2]);				
						//kill(pid,SIGTERM);					
						exit(0);
				}
				else if(data[4]==2){
					printf("Game over:Client won the game\n");
					printf("Client:Client Total=%d\n",data[3]);
					printf("Client:Server Total= %d\n",data[2]);				
						//kill(pid,SIGTERM);					
						exit(0);
				}
					
				}				
			}
}