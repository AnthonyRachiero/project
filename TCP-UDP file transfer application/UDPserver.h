#define BUFFSIZE 256

typedef struct
{
	SOCKET sd;
	int client_len;
	int port;
	struct sockaddr_in server;
	struct sockaddr_in client;
	HANDLE fp;
	char filename[BUFFSIZE];
	HWND hwnd;
	WSAOVERLAPPED overlapped;
	WSABUF filecontents;
	int					BytesRECV;
	int					BytesSEND;
}UDPserverInfo;

typedef struct
{
	int					port;
	int					BytesRECV;
	int					BytesSEND;
	int					client_len;
	int					n;
	SOCKET				sd;
	SOCKET				new_sd;
	struct sockaddr_in	server;
	struct sockaddr_in	client;
	HWND				hwnd;
	char				filename[BUFFSIZE];
	HANDLE				fp;
	WSAOVERLAPPED		overlapped;
	WSABUF				filecontents;

}controlTCPserverInfo;

typedef struct 
{
	UDPserverInfo *USI;
	controlTCPserverInfo *UTSI;
}
UDPthreadinfo;

int UDPserver(UDPserverInfo USI, controlTCPserverInfo UTSI, UDPthreadinfo UTI, HWND hwnd);
DWORD WINAPI ProcessUDPIO(LPVOID p);