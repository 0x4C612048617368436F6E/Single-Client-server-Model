#include"../include/common.h"

void incorrectNumberOfCommandLineArgument(char*message){
	fprintf(stderr,message);
}

void unableToCreateSocket(char*message){
	//create
	fprintf(stderr,message);
}

