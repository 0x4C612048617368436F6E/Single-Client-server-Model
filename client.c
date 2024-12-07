#include"./include/common.h"

//struct to represent the server
typedef struct{
	int serverSocket;
}Server;

void* handleRequest(void* args){
	///receive message here
	int socketID = *(int *)args;
	ssize_t nread;
	char buf[BUF_SIZE];

	nread = read(socketID,buf,BUF_SIZE);
	if(nread == -1){
		printf("Unable to read");
		exit(EXIT_FAILURE);
	}
	printf("%s",buf);

}

char* getUserInput(){
	//getting user input
	//can make use of the getchar();
	static char character[BUF_SIZE];
	char holder = getchar();
	int pointer = 0;

	while(holder != '\n'){
		//append to character
		character[pointer++] = holder;
		holder = getchar();
	}
	character[pointer] = '\0';
	return character;
}

int main(int argc, char**argv){
	//varibales and stuff
	int sfd,s;
	char buf[BUF_SIZE];
	int len;
	ssize_t nread;
	struct addrinfo hints;
	struct addrinfo *result,*rp;
	//check arguments
	if(argc < 3){
		//function handler here
		printf("Number of arguments passed is insufficient");
		exit(EXIT_FAILURE);
	}

	memset(&hints,0,sizeof(struct addrinfo));
	//configure hints
	//This for UDP - Will look at TCP later
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = 0;

	s = getaddrinfo(argv[1],argv[2],&hints,&result);
	//some error checks
	if(s!=0){
		printf("Error occured");
		exit(EXIT_FAILURE);
	}

	//create linked-list
	for(rp = result; rp!=NULL; rp = rp->ai_next){
		sfd = socket(rp->ai_family, rp->ai_socktype,rp->ai_protocol);
		if(sfd == -1) continue;

		//if can connect
		if(connect(sfd,rp->ai_addr,rp->ai_addrlen)!=-1) break;
		//close since success
		close(sfd);
	}
	//free resources -> No longer needing
	freeaddrinfo(result);
	//make sure we actually connect
	if(rp == NULL){
		printf("Could Not connect");
		exit(EXIT_FAILURE);
	}
	//create our own custom stuff
	char *message;
	printf("Enter Username: ");
	//create server instance
	Server server;
	server.serverSocket = sfd;
	pthread_t thread_id;
	int* sfdClient = malloc(sizeof(int));
	sfdClient = &sfd;
	pthread_create(&thread_id,NULL,handleRequest,sfdClient);
	while(1){
		message = getUserInput();
		//printf("Welcome %s\n",message);
		//user will be prompted to enter what to send
		//send username to server
		//pthread_t thread_id;
		//pthread_create(&thread_id,NULL,handleRequest,(void *)&server.serverSocket);
		len = strlen(message)+1;
		if(write(sfd,message,len)!=len){
			printf("Unale to write");
			exit(EXIT_FAILURE);
		}

		//receive message here
		//nread = read(sfd,buf,BUF_SIZE);
		//if(nread == -1){
		//	printf("Unable to read");
		//	exit(EXIT_FAILURE);
		//}
		//printf("%s",buf);
		//
		//
		//
		//create PThread to handl listening heere
		//pthread_t thread_id;
		//pthread_create(&thread_id,NULL,handleRequest,(void *)&server.serverSocket);
	}
	
	exit(EXIT_SUCCESS);

}
