#define BUFFSIZE 256

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
	LPARAM				lParam;
	HDC hdc;


}TCPserverInfo;

int TCPserver(TCPserverInfo TSI, HWND hwnd);
DWORD WINAPI ProcessTCPIO(LPVOID p);
