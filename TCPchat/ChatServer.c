#include "ChatServer.h"
#include <ctype.h>
#include <string.h>
#define BUFLEN 512
/**************************************************************

Source File: ChatServer.c

programmer: Anthony Rachiero
Designer: Anthony Rachiero


Arguments: argc - the number of arguments supplied to this program
				  this program takes 1 argument.

		   argv - the arguments passed to this program. this 
	   			  program accepts a port number as an argument


Notes: This server program waits for client connnections using 
	   TCP functions. it uses Select() to allow multiple clients
	   to be connected and to be differentiated from one another.
	   when a message is received by one of the clients it is
	   sent to all other clients connected to the server, appending
	   the sending clients IP address to the front of the message
	   so the other clients know who its from.


****************************************************************/
int main(int argc, char *argv[])
{
	clientinfo client[FD_SETSIZE];
	int port;
	int arg;
	int nready;
	int maxfd, maxi;
	int listen_socket, new_socket;
	//make an array of sock adders to keep trac of IP's
	struct sockaddr_in server;
	struct sockaddr_in client_addr;
	fd_set rset, allset;
	socklen_t client_len;
	int sockfd;
	int i = 0;
	ssize_t n;
	char *bp, buf[BUFLEN];
	int bytes_to_read;
	int j = 0;
	char newbuf[BUFLEN];


	if(argc < 2)
	{
		fprintf(stderr, "Usage: Port");
		exit(0);
	}
	//get the port from user
	port = atoi(argv[1]);

	//create listening socket
	if((listen_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("coould not create socket");
		exit(2);
	}
	//set socket option to allow socket reuse after exit
	setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &arg, sizeof(arg));

	//bind address to socket
	bzero((char *)&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
		

	if (bind(listen_socket, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		perror("bind failed");
		exit(1);
	}

	listen(listen_socket, 5);

	maxfd = listen_socket;
	maxi = -1;	

	for(i = 0; i < FD_SETSIZE; i++)
	{
		client[i].client = -1;
	}

	FD_ZERO(&allset);
	FD_SET(listen_socket, &allset);
	
	while(1)
	{
		rset = allset;
		nready = select(maxfd + 1, &rset, NULL,NULL,NULL);
		//new client connection
		if(FD_ISSET(listen_socket, &rset))
		{
			printf("new connection");
			fflush(stdout);
			client_len = sizeof(client_addr);
			if((new_socket = accept(listen_socket, (struct sockaddr *) &client_addr, &client_len)) == -1)
			{
				perror("accept failed");
			}
			printf("remote address: %s\n", inet_ntoa(client_addr.sin_addr));
			for(i = 0; i < FD_SETSIZE; i++)
			{
				if(client[i].client < 0)
				{
					client[i].client = new_socket;
					client[i].IPaddress = inet_ntoa(client_addr.sin_addr);
				
					break;
				}
			}
				if(i == FD_SETSIZE)
				{
					perror("too many connections");
					exit(0);
				}
				FD_SET (new_socket, &allset);

				if(new_socket > maxfd)
				{
					maxfd = new_socket;
				}

				if(i > maxi)
				{
					maxi = i;
				}
				if(--nready <= 0)
				{
					continue;
				}
			
		}
		for (i = 0; i <= maxi; i++)	// check all clients for data
     	{

			if((sockfd = client[i].client) < 0)
			{

				continue;
			}
			if (FD_ISSET(sockfd, &rset))
         	{
         		bp = buf;
				bytes_to_read = BUFLEN;
				n = read(sockfd, bp, bytes_to_read);
				printf(" %s \n", buf);
				bp += n;
				bytes_to_read -= n;
			

				sprintf(newbuf , "[%s]: %s", client[i].IPaddress, buf);
				printf("%s \n", newbuf);
				for(j = 0; j < FD_SETSIZE; j++)
				{
					if(client[j].client > 0)
					{
						if(client[i].client != client[j].client)
						{
							printf("sending to client\n");
							send(client[j].client, newbuf, sizeof(newbuf), 0);
						}
					}
				}
				if (n == 0) // connection closed by client
            	{
					printf(" Remote Address:  %s closed connection\n", client[i].IPaddress);
					close(sockfd);
					FD_CLR(sockfd, &allset);
               		client[i].client = -1;
            	}
									            				
				if (--nready <= 0)
				{

            				break; 
				}       // no more readable descriptors
			}
		}
	}
	return 0;
}

