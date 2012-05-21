#include "main.h"

WSAEVENT eventarray[1];
DWORD eventtotal = 1;
DWORD RecvBytes, SendBytes;

extern int packetSize;
extern HDC Ghdc;
int t1 = 0;
int t2 = 0;
/*---------------------------------------------------------------------------------------
--	FUNCTION:		TCPserver

--	DATE:			January 16, 2012
--
--	REVISIONS:		(Date and Description)
--
--  INTERFACE:		int TCPserver(TCPserverInfo TSI, HWND hwnd)
--
--  RETURNS:		void
--
--	DESIGNERS:		Anthony Rachiero
--
--	PROGRAMMER:		Anthony Rachiero
--
--	NOTES: This function acts as a server for a TCP connection, it opens a socket and 
		   waits for the client to connect. a thread is spawned to deal with the receiving
		   duties of the server. after the connection is made the server receives the data
		   being sent by the client, writes it to a file and displays the statistics of the
		   transfer.
---------------------------------------------------------------------------------------*/
int TCPserver(TCPserverInfo TSI, HWND hwnd)
{
	int n = 1;
	int bytes_to_read;
	int ns;
	int err;
	CRITICAL_SECTION crit_section;
	DWORD threadid;
	char port[BUFFSIZE];
	HWND hport, tfile;
	DWORD bytesRecv;
	WSABUF filecontents;
	DWORD BytesWritten = 0;
	
	filecontents.len = PACKETSIZE1;
	filecontents.buf = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 1024);
	
	
	hport = GetDlgItem(hwnd, ID_PORT);
	GetWindowText(hport, port, 50);
	tfile = GetDlgItem(hwnd, ID_FILENAME1);
	GetWindowText(tfile, TSI.filename, 50);

	if(isdigit(*port))
	{
		TSI.port = atoi(port);
	}
	if((TSI.fp = CreateFile(TSI.filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL)) == NULL)
	{
		exit(8);
	}

	InitializeCriticalSection(&crit_section);

	//WSA startup
	wsaInit();
	
	if((TSI.sd = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		exit(1);
	}

	memset((char *)&TSI.server, 0, sizeof(struct sockaddr_in));
	TSI.server.sin_family = AF_INET;
	TSI.server.sin_port = htons(TSI.port);
	TSI.server.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(TSI.sd, (struct sockaddr *)&TSI.server, sizeof(TSI.server)) == SOCKET_ERROR)
	{
		exit(2);
	}
	
	listen(TSI.sd, 5);
	//setup lstneing socket
	if((TSI.new_sd = WSASocket(AF_INET, SOCK_STREAM, 0, NULL,0,WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		exit(22);
	}

	if((eventarray[0] = WSACreateEvent()) == WSA_INVALID_EVENT)
	{
		exit(23);
	}

	TSI.hwnd = hwnd;

	if(CreateThread(NULL, 0, ProcessTCPIO, (void *)&TSI, 0, &threadid) == NULL)
	{
		exit(24);
	}

	eventtotal = 1;

	while(1)
	{

		if((TSI.new_sd = accept (TSI.sd, NULL, NULL)) == INVALID_SOCKET)
		{
			exit(3);
		}
	
		t1 = time(NULL);

		EnterCriticalSection(&crit_section);

		ZeroMemory(&TSI.overlapped, sizeof(WSAOVERLAPPED));

		if((eventarray[1] = WSACreateEvent()) == WSA_INVALID_EVENT)
		{
			err = WSAGetLastError();
			exit (27);
		}

		TSI.overlapped.hEvent = eventarray[0];
		/*
		maybe create socket info struct and fill it out here
		*/
		
		if(WSARecv(TSI.new_sd, &filecontents, 1	, &bytesRecv, 0, &TSI.overlapped, NULL) == SOCKET_ERROR)
		{
			//write to file
			WriteFile(TSI.fp, filecontents.buf, PACKETSIZE1, &BytesWritten, 0);
		}
		


		LeaveCriticalSection(&crit_section);

		if(WSASetEvent(eventarray[0]) == FALSE)
		{
			exit(25);
		}
	}
	
	closesocket(TSI.sd);
	WSACleanup();
	exit(4);

	return 0;
}

DWORD WINAPI ProcessTCPIO(LPVOID p)
{
	DWORD Index;
	DWORD Flags;
	int	err;
	DWORD bytes_transferred = 0;
	TCPserverInfo *TSI;
	FILE* fp;
	int n;
	DWORD bytesRecv;
	char input[PACKETSIZE1];
	HWND statlabel;
	int h = 7777777;
	DWORD flags = 0;
	HANDLE filename;
	DWORD BytesWritten = 0;
	int kbps = 0;
	int filesize = 0;
	int packetsreceived = 0;
	char f[BUFFSIZE];
	char kilo[PACKETSIZE1];
	char sendTime[BUFFSIZE];
	HDC hdc;
	int sendtime = 0;
	int i = 0;
	int bytestobereceived = packetSize + 1;


	TSI = (TCPserverInfo *)p;

	hdc = GetDC(TSI->hwnd);
	while(1)
	{
		if((Index = WSAWaitForMultipleEvents(eventtotal, eventarray, FALSE, WSA_INFINITE, FALSE)) == WSA_WAIT_FAILED)
		{
			Index = WSAGetLastError();
			exit(26);
		}
		
		if(WSAGetOverlappedResult(TSI->new_sd, &TSI->overlapped, &bytesRecv, FALSE, &flags) == FALSE || bytesRecv == 0)
		{
			err = WSAGetLastError();


		}
		if(bytestobereceived <= packetSize)
		{
			t2 = time(NULL);
			sendtime = t2 - t1;
			sprintf(sendTime, "%s %d", "sendtime: ", sendtime);
			TextOut(Ghdc, 400, 320, sendTime, strlen(sendTime));
			if(sendtime != 0)
			{
				kbps = (packetsreceived / sendtime);
				sprintf(kilo, "%d %s", kbps, "Kbps: ");
				TextOut(Ghdc, 400, 360, kilo, strlen(kilo));
			}
			else
			{
				kbps = packetsreceived;
				sprintf(kilo, "%d %s", kbps, "Kbps: ");
				TextOut(Ghdc, 400, 360, kilo, strlen(kilo));
			}
			break;
		}
		/*
		if((Index - WSA_WAIT_EVENT_0) == 0)
		{
			WSAResetEvent(eventarray[0]);
			continue;
		}
		*/

		TSI->filecontents.buf = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, packetSize);
		TSI->filecontents.len = packetSize;

		TSI->client_len = sizeof(TSI->client);



		if(WSARecv(TSI->new_sd, &(TSI->filecontents), 1, NULL, &flags, &(TSI->overlapped), NULL) == SOCKET_ERROR)
		{
			err = WSAGetLastError();
			if(err != ERROR_IO_PENDING)
			{
				break;
			}
		}
		else 
		{
			if(i == 0)
			{
				filesize = atoi(TSI->filecontents.buf);
				if(filesize > 0)
				{
					bytestobereceived = filesize;
					i++;
				}
			}
			bytestobereceived -= packetSize;


			WriteFile(TSI->fp, TSI->filecontents.buf, packetSize, &BytesWritten, 0);
		
			sprintf(f, "%d %s", packetsreceived, " Packets Received");
			TextOut(Ghdc, 400, 300, f, strlen(f));

			sprintf(f, "%s %d %s", "filesize: ", (filesize / 1024), "KB");
			TextOut(Ghdc, 400, 340, f, strlen(f));
			packetsreceived++;
		}
	}
}