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
		//printf("\nClient Thread\n");
		nread = recvfrom(clients[positionInArray].clientSocket,buf,BUF_SIZE,0,(struct sockaddr *)&clients[positionInArray].peer_addr,&clients[positionInArray].peer_addrlen);
		if(nread == -1) {
			continue;
		}else{
			printf("\nReceived from server.. Client thread\n");
		}
		//send message to other clients except client we are handling
		buf[nread] = '\0';
		sprintf(sendToOtherClients,"%s:%s",clients[positionInArray].clientName,buf);
		printf("%s\n",buf);
	}
	free(args);
}

void PrintClientAddress(struct sockaddr *client){
	char ip[INET_ADDRSTRLEN];
    struct sockaddr_in *sa_in = (struct sockaddr_in *)client;
	// Convert the binary IP address to a string
    inet_ntop(AF_INET, &(sa_in->sin_addr), ip, INET_ADDRSTRLEN);

    // Print the IP address and port
    printf("\nIP Address: %s\n", ip);
    printf("\nPort: %d\n", ntohs(sa_in->sin_port));
}

//return 0 if false else 1 if true
int CheckIfClientUserNameAlreadyExist(char username[],int sfd){
	int val = 0;
	for(size_t i = 0; i<sizeof(clients)/sizeof(clients[0]);i++){
		if(strcmp(username,clients[i].clientName)==0 || clients[i].clientSocket == sfd){
			val = 1;
			break;
		}
	}
	return val;
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
		//printf("\nServer Thread\n");
		char host[NI_MAXHOST], service[NI_MAXSERV];
		peer_addrlen = sizeof(struct sockaddr_storage);
		//receiver from client, but client need to connect
		//printf("Nothing here");
	
		//store client information here
		//buf[nread] = '\0';
		//check if array is empty or if client does not alreayd exists
		if(clientCounter == 0){

			nread = recvfrom(sfd,buf,BUF_SIZE,0,(struct sockaddr *)&peer_addr,&peer_addrlen);
			if(nread == -1){
				continue;
			}else{
				struct sockaddr* temp = (struct sockaddr *)&peer_addr;
				PrintClientAddress(temp);
				printf("\nReceived from server.. Main thread\t Socket is:%i\n",sfd);
			}


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
				printf("\nError occured\n");
				
				}else{
				printf("\nOK\n");
				}
		
			}
			//lets send
			//add client to the array
			clients[clientCounter++]=client;
			printf("%i",clientCounter);
			//create thread to
			//pthread_t thread_id;
			int* clientCounterPtr = malloc(sizeof(int));
			*clientCounterPtr = clientCounter-1;
			pthread_create(&thread_id[clientCounter-1],NULL,handleClient,clientCounterPtr);	
		}else{
			//Clients are here already
			//check if client does not already exist
			nread = recvfrom(sfd,buf,BUF_SIZE,0,(struct sockaddr *)&peer_addr,&peer_addrlen);
			if(nread == -1){
				continue;
			}else{
				struct sockaddr* temp = (struct sockaddr *)&peer_addr;
				PrintClientAddress(temp);
				printf("\nReceived from server.. Main thread\tSocket is:%i\n",sfd);
			}
			if(CheckIfClientUserNameAlreadyExist(buf,sfd) == 0){
				//can create instance of the client and increment client
				printf("\nClient does not already exist\n");
			}else{
				printf("\nClient already exist\n");
				//send message to client telling them this and disconnect them
			}
		}
		//}
		//}
		
	}while (1);
	return;
}
