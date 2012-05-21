#include "main.h"
/*---------------------------------------------------------------------------------------
--	FUNCTION:		UDPclient

--	DATE:			January 16, 2012
--
--	REVISIONS:		(Date and Description)
--
--  INTERFACE:		int UDPclient(UDPclientInfo *UCI, controlTCPclientInfo *UTCI, HWND hwnd)
--
--  RETURNS:		void
--
--	DESIGNERS:		Anthony Rachiero
--
--	PROGRAMMER:		Anthony Rachiero
--
--	NOTES: This function first retrieves the port number and host IP that the client
		   will be operating with, it then opens a socket, binds the socket and allocates
		   memory for the structures. there are 2 sockets opened in the UDP client, 1 for
		   TCP control and the other for UDP sending.
---------------------------------------------------------------------------------------*/
int UDPclient(UDPclientInfo *UCI, controlTCPclientInfo *UTCI, HWND hwnd)
{
	HWND hport, hhost;
	char port[BUFFSIZE];
	char hostname[BUFFSIZE];
	char input[20] = "test data";
	DWORD flags = 0;
	wsaInit();

	hhost = GetDlgItem(hwnd, ID_HOSTNAME);
	GetWindowText(hhost, hostname, 50);
	hport = GetDlgItem(hwnd, ID_PORT);
	GetWindowText(hport, port, 50);

	if(isdigit(*port))
	{
		UCI->port = atoi(port);
		UTCI->port = atoi(port);// might not work
	}
	//create UDP socket for data and tcp socket for control
	if((UCI->sd = WSASocket(PF_INET, SOCK_DGRAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == -1)
	{
		exit(13);
	}
	if((UTCI->sd = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == -1)
	{
		exit(13);
	}


	//store server informatiion for UDP and tcp server
	memset((char *)&UCI->server, 0, sizeof(UCI->server));
	UCI->server.sin_family = AF_INET;
	UCI->server.sin_port = htons(UCI->port);

	memset((char *)&UTCI->server, 0, sizeof(struct sockaddr_in));
	UTCI->server.sin_family = AF_INET;
	UTCI->server.sin_port = htons(UTCI->port);


	if((UCI->hp = gethostbyname(hostname)) == NULL)
	{
		exit(16);
	}
	if((UTCI->hp = gethostbyname(hostname)) == NULL)
	{
		exit(3);
	}

	memcpy((char *)&UCI->server.sin_addr, UCI->hp->h_addr, UCI->hp->h_length);
	memcpy((char *)&UTCI->server.sin_addr, UTCI->hp->h_addr, UTCI->hp->h_length);
/*
	if (bind (UCI->sd, (struct sockaddr *)&UCI->server, sizeof(UCI->server)) == -1)
	{
		exit(15);
	}
	*/
	UCI->client_len = sizeof(UCI->client);
	UCI->server_len = sizeof(UCI->server);

	
	return 0;
}
