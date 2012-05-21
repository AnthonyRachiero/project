
typedef struct 
{
	SOCKET sd;
    struct sockaddr_in server;
	struct sockaddr_in client;
	int server_len;
	int client_len;
	struct hostent *hp;
	int port;
	char *host;
}UDPclientInfo;

typedef struct 
{
	SOCKET sd;
	struct sockaddr_in server;
	struct hostent *hp;
	int port;
	char *host;
}controlTCPclientInfo;

int UDPclient(UDPclientInfo *UCI, controlTCPclientInfo *UTCI, HWND hwnd);