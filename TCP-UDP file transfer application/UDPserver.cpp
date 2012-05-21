#include "main.h"

WSAEVENT eventarray2[1];
DWORD eventtotal2 = 1;

extern HDC Ghdc;
extern int packetSize;
extern int t1;
extern int t2;
/*---------------------------------------------------------------------------------------
--	FUNCTION:		UDPserver

--	DATE:			January 16, 2012
--
--	REVISIONS:		(Date and Description)
--
--  INTERFACE:		int UDPserver(UDPserverInfo USI, controlTCPserverInfo UTSI, UDPthreadinfo UTI, HWND hwnd)
--
--  RETURNS:		void
--
--	DESIGNERS:		Anthony Rachiero
--
--	PROGRAMMER:		Anthony Rachiero
--
--	NOTES: This function acts as a server for a UDP connection, it opens a socket and 
		   waits for the client to connect. a thread is spawned to deal with the receiving
		   duties of the server. after the connection is made the server receives the data
		   being sent by the client, writes it to a file and displays the statistics of the
		   transfer. the UDP server also opens a TCP connection to accept a control packet
		   that contains the amount of bytes to be read, so that the UDP connection knows
		   when to stop reading.
---------------------------------------------------------------------------------------*/
int UDPserver(UDPserverInfo USI, controlTCPserverInfo UTSI, UDPthreadinfo UTI, HWND hwnd)
{
	HWND hport, tfile;
	char port[BUFFSIZE];
	FILE *fp;
	WSABUF filecontents;
	CRITICAL_SECTION crit_section;
	DWORD threadid;
	int err;
	DWORD flags = 0;
	DWORD bytesread;
	DWORD BytesWritten;
	DWORD bytesRecv;

	filecontents.len = PACKETSIZE1;
	filecontents.buf = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 1024);
	
	hport = GetDlgItem(hwnd, ID_PORT);
	GetWindowText(hport, port, 50);
	tfile = GetDlgItem(hwnd, ID_FILENAME1);
	GetWindowText(tfile, USI.filename, 50);

	if((USI.fp = CreateFile(USI.filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL)) == NULL)
	{
		exit(8);
	}

	if(isdigit(*port))
	{
		USI.port = atoi(port);
		UTSI.port = atoi(port);
	}

	InitializeCriticalSection(&crit_section);

	wsaInit();



	if((USI.sd = WSASocket(PF_INET, SOCK_DGRAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		exit(11);
	}
	if((UTSI.sd = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		exit(1);
	}

	//store local server info for UDP and TCP sockets
	memset((char *)&USI.server,0, sizeof(USI.server));
	USI.server.sin_family = AF_INET;
	USI.server.sin_port = htons(USI.port);
	USI.server.sin_addr.s_addr = htonl(INADDR_ANY);

	memset((char *)&UTSI.server, 0, sizeof(struct sockaddr_in));
	UTSI.server.sin_family = AF_INET;
	UTSI.server.sin_port = htons(UTSI.port);
	UTSI.server.sin_addr.s_addr = htonl(INADDR_ANY);

	//binc the TCP and UDP sockets
	if(bind(USI.sd, (struct sockaddr *)&USI.server, sizeof(USI.server)) == SOCKET_ERROR)
	{
		exit(2);
	}
	if(bind(UTSI.sd, (struct sockaddr *)&UTSI.server, sizeof(UTSI.server)) == SOCKET_ERROR)
	{
		exit(2);
	}
	//create accept socket and listen for connections
	listen(UTSI.sd, 5);
	if((UTSI.new_sd = WSASocket(AF_INET, SOCK_STREAM, 0, NULL,0,WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		exit(22);
	}

	if((eventarray2[0] = WSACreateEvent()) == WSA_INVALID_EVENT)
	{
		exit(23);
	}

	USI.hwnd = hwnd;
	UTSI.hwnd = hwnd;

	//pass both structs into the thread
	//memset((char *)&UTII, 0, sizeof(UTII));

	UTI.USI = &USI;
	UTI.UTSI = &UTSI;
	
	if(CreateThread(NULL, 0, ProcessUDPIO, (void *)&UTI, 0, &threadid) == NULL)
	{
		exit(24);
	}

	eventtotal2 = 1;


	while(1)
	{

		if((UTSI.new_sd = accept (UTSI.sd, NULL, NULL)) == INVALID_SOCKET)
		{
			exit(3);
		}
		t1 = time(NULL);
		EnterCriticalSection(&crit_section);

		ZeroMemory(&USI.overlapped, sizeof(WSAOVERLAPPED));
		if((eventarray2[1] = WSACreateEvent()) == WSA_INVALID_EVENT)
		{
			err = WSAGetLastError();
			exit (27);
		}

		UTSI.overlapped.hEvent = eventarray2[0];
		USI.overlapped.hEvent = eventarray2[0];
		

		if(WSARecv(UTSI.new_sd, &filecontents, 1	, &bytesRecv, 0, &UTSI.overlapped, NULL) == SOCKET_ERROR)
		{
			//write to file
			WriteFile(USI.fp, filecontents.buf, PACKETSIZE1, &BytesWritten, 0);
		}
	


		LeaveCriticalSection(&crit_section);

		if(WSASetEvent(eventarray2[0]) == FALSE)
		{
			exit(25);
		}
	}





	closesocket(USI.sd);
	WSACleanup();
	exit(17);

	return 0;
}
DWORD WINAPI ProcessUDPIO(LPVOID p)
{
	DWORD Index;
	DWORD Flags;
	int	err;
	DWORD bytes_transferred = 0;
	FILE* fp;
	int n;
	DWORD bytesRecv;
	char input[PACKETSIZE1];
	UDPserverInfo *USI;
	UDPthreadinfo *UTI;
	controlTCPserverInfo *UTSI;
	HANDLE filename;
	DWORD BytesWritten = 0;
	DWORD flags = 0;
	int BytesToBeRecv = 0;
	int kbps = 0;
	int filesize = 0;
	int packetsreceived = 0;
	char f[BUFFSIZE];
	char kilo[PACKETSIZE1];
	char sendTime[BUFFSIZE];
	HDC hdc;
	int sendtime = 0;
	int i = 0;
	int bytestobereceived = 1025;
	int msgboxid;

	UTI = (UDPthreadinfo *)p;

	USI = UTI->USI;
	UTSI = UTI->UTSI;

	while(1)
	{
		if((Index = WSAWaitForMultipleEvents(eventtotal2, eventarray2, FALSE, WSA_INFINITE, FALSE)) == WSA_WAIT_FAILED)
		{
			Index = WSAGetLastError();
			exit(26);
		}


		/*
		if((Index - WSA_WAIT_EVENT_0) == 0)
		{
			WSAResetEvent(eventarray[0]);
			continue;
		}
		*/
		if(WSAGetOverlappedResult(UTSI->new_sd, &UTSI->overlapped, &bytesRecv, FALSE, &flags) == FALSE || bytesRecv == 0)
		{
			err = WSAGetLastError();
		}

		if(USI->BytesRECV == 0)
		{
			USI->BytesRECV = bytes_transferred;
			USI->BytesSEND = 0;
		}
		else
		{
			USI->BytesSEND += bytes_transferred;
		}

		USI->filecontents.buf = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, packetSize);
		USI->filecontents.len = packetSize;
		UTSI->filecontents.buf = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, packetSize);
		UTSI->filecontents.len = packetSize;
		if(WSARecv(UTSI->new_sd, &(UTSI->filecontents), 1, NULL, &flags, &(UTSI->overlapped), NULL) == SOCKET_ERROR)
		{
			err = WSAGetLastError();
			if(err != ERROR_IO_PENDING)
			{
				break;
			}
		}
		BytesToBeRecv = atoi(UTSI->filecontents.buf);
		filesize = BytesToBeRecv;
		while(BytesToBeRecv >= 1024)
		{
			if(WSARecvFrom(USI->sd, &USI->filecontents, 1, &bytesRecv, &flags,NULL,NULL,&USI->overlapped, NULL) == SOCKET_ERROR)
			{
				err = WSAGetLastError();
				if(err != ERROR_IO_PENDING)
				{
					
				}
			}
			WriteFile(USI->fp, USI->filecontents.buf, packetSize, &BytesWritten, 0);

			sprintf(f, "%d %s", packetsreceived, " Packets Received");
			TextOut(Ghdc, 400, 300, f, strlen(f));

			sprintf(f, "%s %d %s", "filesize: ", (filesize / 1024), "KB");
			TextOut(Ghdc, 400, 340, f, strlen(f));

			packetsreceived++;
			BytesToBeRecv -= packetSize;
		}
		//print stats
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
	}
}