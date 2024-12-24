#ifndef SERVER
#define SERVER
#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netdb.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#define BUF_SIZE 1024
void UnableToCreateSocket(char*message);
void serverInitialization(int argc, char** argv);
#endif
