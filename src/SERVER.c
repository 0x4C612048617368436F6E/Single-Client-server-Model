#include"../include/common.h"
//create a struct to hold a typical client
typedef struct{
	int clientID;
	char clientName[BUF_SIZE];
	int clientSocket;
	struct sockaddr_storage peer_addr; //client addr
	socklen_t peer_addrlen; //client addr len
}Client;

Client clients[10];
int clientCounter = 0;
pthread_t thread_id[10];

//function to handle client
void* handleClient(void* args){
	ssize_t nread;
	char buf[BUF_SIZE];
	char sendToOtherClients[BUF_SIZE];
	//handle the client
	int positionInArray = *(int *)args;
	//receive from client
	while(1){
		nread = recvfrom(clients[positionInArray].clientSocket,buf,BUF_SIZE,0,(struct sockaddr *)&clients[positionInArray].peer_addr,&clients[positionInArray].peer_addrlen);
		if(nread == -1) continue;
		//send message to other clients except client we are handling
		buf[nread] = '\0';
		sprintf(sendToOtherClients,"%s:%s",clients[positionInArray].clientName,buf);
		//for(size_t i=0; i<sizeof(clients)/sizeof(clients[0]);i++){
		//	if(sendto(clients[i].clientSocket,sendToOtherClients,strlen(sendToOtherClients),0,(struct sockaddr*)&clients[i].peer_addr,clients[i].peer_addrlen)!=strlen(sendToOtherClients)){
		//		printf("Error Occured");
		//	}
		//}
	}
	free(args);
}

void serverInitialization(int argc, char**argv){
	int sfd,s;
	char buf[BUF_SIZE];
	char welcomeMsg[BUF_SIZE];
	ssize_t nread;
	socklen_t peer_addrlen;
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	struct sockaddr_storage peer_addr;


	//check if argc is less than 2
	if(argc<2){
		//call function here
		printf("Argument provided is not sufficient enough");
		exit(EXIT_FAILURE);
	}

	//reset the hints memory
	memset(&hints,0,sizeof(struct addrinfo));
	//set hint members
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 0;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	//lets create the linkedlist
	if(getaddrinfo(NULL,argv[1],&hints,&result) != 0){
		//will put this inside of function later
		printf("Error Occured");
		exit(EXIT_FAILURE);
	}
	//loop over the linked list
	for(rp = result; rp!= NULL; rp = rp->ai_next){
		//create socket
		sfd = socket(rp->ai_family,rp->ai_socktype,rp->ai_protocol);
		//check if bad
		if(sfd == -1) continue;
		//otherwise we have successfully created a socket
		//next try bind
		if(bind(sfd,rp->ai_addr,rp->ai_addrlen) == 0) break; //able to bind
		
		//if not able to bind, close the integer that reference the socker
		close(sfd);
	}
	//free the result, as no longer need
	freeaddrinfo(result);
	//to ensure we actually created a socket
	if(!(rp)){
		//create function for this later
		printf("Could not bind");
		exit(EXIT_FAILURE);
	}

	//Below will handle the inter-chnage of information between server and client
	printf("\nServer is listening\n");
	do{
		char host[NI_MAXHOST], service[NI_MAXSERV];
		peer_addrlen = sizeof(struct sockaddr_storage);
		//receiver from client, but client need to connect
		printf("Nothing here");
		nread = recvfrom(sfd,buf,BUF_SIZE,0,(struct sockaddr *)&peer_addr,&peer_addrlen);
		if(nread == -1) continue;
		//store client information here
		buf[nread] = '\0';
		Client client;
		client.clientID = clientCounter + 1;
		//client name
		sprintf(client.clientName,"%s",buf);
		client.clientName[strlen(client.clientName)] = '\0';
		client.clientSocket = sfd;
		//handle situation where client laves server, let others known
		client.peer_addr = peer_addr;
		client.peer_addrlen = peer_addrlen;
		//check if bad
		fprintf(stdout,"%s joined the chat",client.clientName);
		int pointer = strlen(welcomeMsg);
		char welcomeMsg[BUF_SIZE];
		sprintf(welcomeMsg,"Hello %s",buf);
		welcomeMsg[strlen(welcomeMsg)] = '\0';
		s = getnameinfo((struct sockaddr *) &client.peer_addr, client.peer_addrlen, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV);
		//check if value is good, if true, print all the values
		if(s == 0){
			//printf("Received %zd bytes from %s:%s\n",nread,host,service);
			//printf("%s Joined the server\n",buf);
			//send welcome to user
			if(sendto(client.clientSocket,welcomeMsg,strlen(welcomeMsg),0,(struct sockaddr *)&client.peer_addr,client.peer_addrlen)!=strlen(welcomeMsg)){
				//handle error later
				printf("Error occured");
			}
			/*
			//send {Name of client} Joined server to all clients in the current session
			char* sendToOtherClients;
			sprintf(sendToOtherClients,"%s joined the server",client.clientName);

			for(int i=0; i<sizeof(clients)/sizeof(clients[0]);i++){
				if(sendto(clients[i].clientSocket,sendToOtherClients,strlen(sendToOtherClients),0,(struct sockaddr*)&clients[i].peer_addr,clients[i].peer_addrlen)!=strlen(sendToOtherClients)){
					printf("An error occured");
				}
			}*/
		}else{
			printf("Error  Occured");
		
		}
		//lets send
		//add client to the array
		clients[clientCounter++]=client ;
		//create thread to
		//pthread_t thread_id;
		int* clientCounterPtr = malloc(sizeof(int));
		*clientCounterPtr = clientCounter-1;
		pthread_create(&thread_id[clientCounter-1],NULL,handleClient,clientCounterPtr);
		
	}while(1);

	//Next and most important thing is to handle multiple users at the same time

	return;
}
