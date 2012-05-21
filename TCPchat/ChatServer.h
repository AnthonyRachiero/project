#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFSIZE 256

typedef struct
{
	int client;
	char *IPaddress;

}clientinfo;
