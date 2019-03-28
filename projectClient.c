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
#include <signal.h>

int main(int argc, char *argv[]){
	int sock, connClient, port;
	char message[255];
	char str[6];
	int data[5];
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
	 sscanf(argv[2],"%d",&port);
	 clientConfig.sin_port = htons((uint16_t)port);
	 
	 if(inet_pton(AF_INET,argv[1],&clientConfig.sin_addr)<0){
		   fprintf(stderr, " inet_pton() has failed\n");
			exit(2);
	 }
	 printf("Port Number:%d\n",port);
	 
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
	
		int pid;
		pid=fork();
		//if parent then read servers mesage
			while(1){
			if((recv(sock,&data,sizeof(data),0))>0){
				if(data[4]!=0 ){
				printf("Game on\n");
				printf("Client playing\n");
				printf("Press enter to continue or Bye to exit\n");
				gets(str);
				if(strcmp(str,"Bye")==0){
					data[4]=0;
					send(sock,&data,sizeof(data),0);
					kill(pid,SIGTERM);					
					exit(0);
					}
				
				printf("*****************************************\n");
				printf("Client:Server Score=%d\n",data[0]);
				printf("Client:Client Score=%d\n",data[1]);
				//increase client score
				data[1] = data[1]+1;
				printf("Client:Client Score after increment=%d\n",data[1]);
				//increase the client total
				data[3] = data[3]+1;
				printf("Client:Client Score total=%d\n",data[3]);
				printf("Client:Server Total= %d\n",data[2]);				
				printf("*****************************************\n");
				send(sock,&data,sizeof(data),0);
				}
				else{
						printf("Server Exited\n");
						exit(0);
					}
				}				
			}
}