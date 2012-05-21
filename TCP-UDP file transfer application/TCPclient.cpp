#include "main.h"
/*---------------------------------------------------------------------------------------
--	FUNCTION:		TCPclient

--	DATE:			January 16, 2012
--
--	REVISIONS:		(Date and Description)
--
--  INTERFACE:		int TCPclient(TCPclientInfo *TCI, HWND hwnd)
--
--  RETURNS:		void
--
--	DESIGNERS:		Anthony Rachiero
--
--	PROGRAMMER:		Anthony Rachiero
--
--	NOTES: This function first retrieves the port number and host IP that the client
		   will be operating with, it then opens a socket, binds the socket and allocates
		   memory for the structures.
---------------------------------------------------------------------------------------*/
int TCPclient(TCPclientInfo *TCI, HWND hwnd)
{
	HWND hport, hhost;
	char input[BUFFSIZE];
	char hostname[BUFFSIZE];
	char test[100] = "test data";
	
	wsaInit();

	hhost = GetDlgItem(hwnd, ID_HOSTNAME);
	GetWindowText(hhost, hostname, 50);
	hport = GetDlgItem(hwnd, ID_PORT);
	GetWindowText(hport, input, 50);

	if(isdigit(*input))
	{
		TCI->port = atoi(input); // might not work
	}

	if((TCI->sd = WSASocket(AF_INET, SOCK_STREAM, 0, NULL,0,WSA_FLAG_OVERLAPPED)) == -1)
	{
		exit(2);
	}
	memset((char *)&TCI->server, 0, sizeof(struct sockaddr_in));
	TCI->server.sin_family = AF_INET;
	TCI->server.sin_port = htons(TCI->port);
	
	//get host name
	TCI->host = hostname; //maybe wont work? char* to char[]
	if((TCI->hp = gethostbyname(TCI->host)) == NULL)
	{
		exit(3);
	}

	
	memcpy((char *)&TCI->server.sin_addr, TCI->hp->h_addr, TCI->hp->h_length);

	//connect to server --DONE ASYNCHRONOUSLY?

	//send(TCI.sd, test, 100, NULL);

	return 0;
}