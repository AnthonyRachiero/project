typedef struct 
{
	SOCKET sd;
	struct sockaddr_in server;
	struct hostent *hp;
	int port;
	char *host;
}TCPclientInfo;


int TCPclient(TCPclientInfo *TCI, HWND hwnd);