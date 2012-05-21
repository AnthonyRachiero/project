#include "ChatClient.h"
#include <pthread.h>
#include <string.h>

typedef struct
{
	int sd;
}threadinfo;

void *TCPread(void *p);

char *filesave;


/*******************************************************

Source File: ChatClient.c

Programmer: Anthony Rachiero
Designer: Anthony Rachiero

Arguments: argc - this function takes 3 arguments
		   argv - IP address, Port, filesave(yes/no)

Notes: This client program connects to a server specifying
 	   a port number and IP address to connect to. a filesave
	   option is also specified in the arguments. once this
	   program is connected to a server it will be able to
	   send messages to all other clients connected to that 
	   server, and display any messages that are being sent
       by other clients.



*******************************************************/
int main(int argc, char *argv[])
{
	int port;
	char *ip_addr;
	int sd;
	struct sockaddr_in server;
	char sbuf[BUFLEN];
	struct hostent *hp;
	pthread_t threadid;
	threadinfo t1;
	FILE *fp;
	int s;
	char newbuf[BUFLEN];

	if(argc < 4)
	{
		perror("USAGE: IP PORT FILESAVE(yes/no)");
	}
	ip_addr = argv[1];
	port = atoi(argv[2]);
	filesave = argv[3];


	if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket failed");
		exit(0);
	}
	bzero((char *)&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	if((hp = gethostbyname(ip_addr)) == NULL)	
	{
		perror("get host failed");
		exit(0);
	}
	bcopy(hp->h_addr,(char *)&server.sin_addr, hp->h_length);

	if(connect(sd,(struct sockaddr *)&server, sizeof(server)) == -1)
	{
		perror("connect failed");
		exit(0);
	}
	t1.sd = sd;
	pthread_create(&threadid, NULL, TCPread, (void*)&t1);
	//pthread_join(thread1, NULL);

	
	while(1)
	{
		fgets(sbuf, BUFLEN, stdin);
		if(strcmp(filesave, "yes") == 0)
		{

			fp = fopen("ChatLog.txt","a+");
			s = fprintf(fp, "%s", sbuf);
			fclose(fp);
		}
		send(sd, sbuf, BUFLEN, 0);
	}
	return 0;
}

void *TCPread(void *p)
{
	int bytes_to_read = BUFLEN;
	char *bp, buf[BUFLEN];
	threadinfo *t1;
	FILE *fp;
	int s = 0;
	t1 = (threadinfo *)p;



	bp = buf;
	while(read(t1->sd, bp, bytes_to_read))
	{
		printf("%s", buf);
		if(strcmp(filesave, "yes") == 0)
		{

			fp = fopen("ChatLog.txt","a+");
			s = fprintf(fp, "%s", buf);
			fclose(fp);
		}
	}
	return 0;
}




