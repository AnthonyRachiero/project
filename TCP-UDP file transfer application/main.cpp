/*
 STUFF TO WRITE
 - comments, finish UDP, write report, gather stats <- fuck this shit
*/
#include "main.h"

HDC Ghdc;
int sendtime;
int packetSize = 1024;
int timesToSend = 1;
/*---------------------------------------------------------------------------------------
--	SOURCE FILE:	Main.cpp 
--
--	PROGRAM:		4985Ass2.exe	
--
--	DATE:			march 3, 2012
--
--	DESIGNERS:		Anthony Rachiero
--
--	PROGRAMMER:		Anthony Rachiero
--
--	NOTES: this program acts as either a server or a client for a TCP or UDP file transfer.
		   the user selects either a UDP or TCP connection and a file to send along with 
		   a few other options. the user will send the file and the server will write the
		   contents to a file of its own and display statistics or the transfer itself.
---------------------------------------------------------------------------------------*/
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
     static TCHAR szAppName[] = TEXT ("HelloWin") ;
     HWND         hwnd ;
     MSG          msg ;
     WNDCLASS     wndclass ;

     wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
     wndclass.lpfnWndProc   = WndProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 0 ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
     wndclass.lpszMenuName  = NULL ;
     wndclass.lpszClassName = szAppName ;

     if (!RegisterClass (&wndclass))
     {
          MessageBox (NULL, TEXT ("This program requires Windows NT!"), 
                      szAppName, MB_ICONERROR) ;
          return 0 ;
     }
     
     hwnd = CreateWindow (szAppName,                  // window class name
                          TEXT ("The Program that always works"), // window caption
                          WS_OVERLAPPEDWINDOW,        // window style
                          CW_USEDEFAULT,              // initial x position
                          CW_USEDEFAULT,              // initial y position
                          CW_USEDEFAULT,              // initial x size
                          CW_USEDEFAULT,              // initial y size
                          NULL,                       // parent window handle
                          NULL,                       // window menu handle
                          hInstance,                  // program instance handle
                          NULL) ;                     // creation parameters
     
     ShowWindow (hwnd, iCmdShow) ;
     UpdateWindow (hwnd) ;
     
     while (GetMessage (&msg, NULL, 0, 0))
     {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
     }
     return msg.wParam ;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
     HDC         hdc ;
     PAINTSTRUCT ps ;
     RECT        rect ;
     HMENU		 hmenu ;
	 HANDLE		 sendButton, portnum, numfield, filename, hostname;
	 static int	 protocol = ID_PROTOCOL_UDP;
	 int		 type = ID_TYPE_CLIENT;
	 int		 err = 0;
	 static TCPclientInfo TCI;
	 static controlTCPclientInfo UTCI;
	 static UDPclientInfo UCI;
	 static controlTCPserverInfo UTSI;
	 static TCPserverInfo TSI;
	 static UDPserverInfo USI;
	 static UDPthreadinfo UTI;
	 char		 input[BUFFSIZE];

     switch (message)
     {
     case WM_CREATE:
		  hdc = GetDC(hwnd);
		  GetClientRect (hwnd, &rect);
		  Ghdc = hdc;
		  hmenu = LoadMenu(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDR_MENU1));
          SetMenu(hwnd, hmenu);

		  sendButton = createInputbutton(hwnd, lParam, 50, 130, 200, 30);
          portnum = createInputfield(hwnd, lParam, 50, 260, 200, 30, ID_PORT, "7000");
		  filename = createInputfield(hwnd, lParam, 50, 0, 200, 30, ID_FILENAME1, "test.txt");
		  hostname = createInputfield(hwnd, lParam, 300, 50, 200, 30, ID_HOSTNAME, "127.0.0.1");

		  TSI.lParam = lParam;

		  if( err != 0 )
		  {
			  exit(1);
		  }
          return 0 ;
	 case WM_COMMAND:
		  hdc = GetDC(hwnd);
		  switch(LOWORD(wParam))
		  {
	        case ID_PROTOCOL_TCP:
				protocol = ID_PROTOCOL_TCP;
				break;
			case ID_PROTOCOL_UDP:
				protocol = ID_PROTOCOL_UDP;
				break;
			case ID_TYPE_CLIENT:
				client(protocol, &TCI, &UCI, &UTCI, hwnd);
				break;
			case ID_TYPE_SERVER:
				server(protocol, TSI, USI, UTSI, UTI, hwnd);
				break;
			case ID_SEND:
				sendfile(protocol, TCI, UCI, UTCI, hwnd);
				break;
			case ID_PACKETSIZE_4096:
				packetSize = 4096;
				break;
			case ID_PACKETSIZE_1024:
				packetSize = 1024;
				break;
			case ID_PACKETSIZE_20KBYTES:
				packetSize = 20480;
				break;
			case ID_PACKETSIZE_60KBYTES:
				packetSize = 61440;
				break;
			case ID_NUMBEROFTIMETOSEND_1:
				timesToSend = 1;
				break;
			case ID_NUMBEROFTIMETOSEND_10:
				timesToSend = 10;
				break;
			case ID_NUMBEROFTIMETOSEND_50:
				timesToSend = 50;
				break;
			case ID_NUMBEROFTIMETOSEND_100:
				timesToSend = 100;
				break;
		  }
		  return 0;
     case WM_PAINT:
          hdc = BeginPaint (hwnd, &ps) ;
    
				  		
		  TextOut(hdc,260,260,"PortNumber", 10);
		  TextOut(hdc,260,0, "filename", 8);
		  TextOut(hdc,510,50, "hostname", 8);

          
          EndPaint (hwnd, &ps) ;
          return 0 ;
          
     case WM_DESTROY:
          PostQuitMessage (0) ;
          return 0 ;
     }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
}
/*---------------------------------------------------------------------------------------
--	FUNCTION:		sendfile

--	DATE:			march 3, 2012
--
--	REVISIONS:		(Date and Description)
--
--  INTERFACE:		int sendfile(int protocol, TCPclientInfo TCI, UDPclientInfo UCI, controlTCPclientInfo UTCI, HWND hwnd)
--
--  RETURNS:		0 if successful
--
--	DESIGNERS:		Anthony Rachiero
--
--	PROGRAMMER:		Anthony Rachiero
--
--	NOTES: this function reads the file specified by the client and sends it to the server
		   via TCP or UDP sockets.
---------------------------------------------------------------------------------------*/
//DONE ASYNCHRONOUSLY
int sendfile(int protocol, TCPclientInfo TCI, UDPclientInfo UCI, controlTCPclientInfo UTCI, HWND hwnd)
{
	FILE *fp;
	HWND tfile;
	char fileinput[BUFFSIZE];
	char input[PACKETSIZE4];
	WSABUF filecontents;
	WSABUF controlsize;
	WSABUF UDPclose;
	int ns = 0;
	DWORD byteSend;
	DWORD bytestoread = packetSize;
	DWORD byteread = 0;
	DWORD flags = 0;
	HANDLE filename;
	TCHAR *s = TEXT("test.txt");
	DWORD filesize;
	DWORD bytesrecv;
	WSAOVERLAPPED overlapped;
	int i = 0;
	int err;
	int t1;
	int t2;
	int j;
	int msgboxid = 0;

	filecontents.len = packetSize;
	memset((char *)input, 0, sizeof(input));
	filecontents.buf = input;
	UDPclose.len = packetSize;
	UDPclose.buf = input;

	tfile = GetDlgItem(hwnd, ID_FILENAME1);
	GetWindowText(tfile, fileinput, 50);

	if((filename = CreateFile(fileinput, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) != INVALID_HANDLE_VALUE)
	{
		if(protocol == ID_PROTOCOL_TCP)
		{
			if(connect (TCI.sd, (struct sockaddr *)&TCI.server, sizeof(TCI.server)) == -1)
			{
				exit(4);
			}
			//get the filesize
			filesize = GetFileSize(filename, NULL);
			filesize *= timesToSend;

			//store the filesize in the buffer
			controlsize.len = PACKETSIZE1;
			controlsize.buf = input;
			sprintf(input, "%d" ,filesize);

			//send a control packet with the filesize
			send(TCI.sd, controlsize.buf , PACKETSIZE1, NULL);

			//read and send the file
			while(ReadFile(filename, input, packetSize, &byteread, 0))
			{
				j = 0;
				if(byteread == 0)
				{
					break;
				}
				//send the file(number of times to send)
				while(j < timesToSend)
				{
					send(TCI.sd, filecontents.buf, byteread, NULL);
					j++;
				}
			}
		
		}
		else
		{
			//get file size
			filesize = GetFileSize(filename, NULL);
			filesize *= timesToSend;

			//store filesize in buffer
			controlsize.len = PACKETSIZE1;
			controlsize.buf = input;
			sprintf(input, "%d" ,filesize);

			if(connect (UTCI.sd, (struct sockaddr *)&UTCI.server, sizeof(UTCI.server)) == SOCKET_ERROR)
			{
				exit(4);
			}
			//send control TCP packet with amount of bytes to be read
			send(UTCI.sd, controlsize.buf , PACKETSIZE1, NULL);
			
			while(ReadFile(filename, input, packetSize, &byteread, 0))
			{
				j = 0;
				//send file (number of times to send
				while(j < timesToSend)
				{
					sendto(UCI.sd, filecontents.buf, byteread, flags, (struct sockaddr *)&UCI.server, UCI.server_len); 
					j++;
				}
			}
			
		}
	}
	return 0;
}
/*---------------------------------------------------------------------------------------
--	FUNCTION:		client

--	DATE:			march 3, 2012
--
--	REVISIONS:		(Date and Description)
--
--  INTERFACE:		int client(int protocol, TCPclientInfo *TCI, UDPclientInfo *UCI, controlTCPclientInfo *UTCI, HWND hwnd)
--
--  RETURNS:		0 if successful
--
--	DESIGNERS:		Anthony Rachiero
--
--	PROGRAMMER:		Anthony Rachiero
--
--	NOTES: this function simply determines which protocol is being used and calls client functions
		   based on that information
---------------------------------------------------------------------------------------*/
int client(int protocol, TCPclientInfo *TCI, UDPclientInfo *UCI, controlTCPclientInfo *UTCI, HWND hwnd)
{
	if(protocol == ID_PROTOCOL_TCP)
	{
		TCPclient(TCI, hwnd);
	}else
	{
		UDPclient(UCI, UTCI, hwnd);
	}
	return 0;
}
/*---------------------------------------------------------------------------------------
--	FUNCTION:		server

--	DATE:			march 3, 2012
--
--	REVISIONS:		(Date and Description)
--
--  INTERFACE:		int server(int protocol, TCPserverInfo TSI, UDPserverInfo USI, controlTCPserverInfo UTSI, 
								UDPthreadinfo UTI, HWND hwnd)
--
--  RETURNS:		0 if successful
--
--	DESIGNERS:		Anthony Rachiero
--
--	PROGRAMMER:		Anthony Rachiero
--
--	NOTES: this function determines which protocol is being used and calls either TCP
		   or UDP server functions based on that information
---------------------------------------------------------------------------------------*/
int server(int protocol, TCPserverInfo TSI, UDPserverInfo USI, controlTCPserverInfo UTSI, UDPthreadinfo UTI, HWND hwnd)
{
	if(protocol == ID_PROTOCOL_TCP)
	{
		TCPserver(TSI, hwnd);
	}
	else
	{
		UDPserver(USI, UTSI, UTI, hwnd);
	}
	return 0;
}

/*---------------------------------------------------------------------------------------
--	FUNCTION:		createInputbutton

--	DATE:			march 3, 2012
--
--	REVISIONS:		(Date and Description)
--
--  INTERFACE:		HANDLE createInputbutton(HWND hwnd, LPARAM lParam, int x, int y, int x2, int y2)
--
--  RETURNS:		a handle to the window
--
--	DESIGNERS:		Anthony Rachiero
--
--	PROGRAMMER:		Anthony Rachiero
--
--	NOTES: this is a wrapper function to create a button within the operating window
---------------------------------------------------------------------------------------*/
HANDLE createInputbutton(HWND hwnd, LPARAM lParam, int x, int y, int x2, int y2)
{
	HANDLE inputwindow;
	inputwindow = CreateWindow  (TEXT("button"),
					TEXT ("SEND"),
					WS_VISIBLE | WS_BORDER | WS_CHILD,       
					x,              
					y,             
				    x2,              
					y2,              
					hwnd,                       
					(HMENU)ID_SEND,                      
					((LPCREATESTRUCT)lParam)->hInstance,                 
					NULL);	
	return inputwindow;
}
/*---------------------------------------------------------------------------------------
--	FUNCTION:		createInputLabel

--	DATE:			march 3, 2012
--
--	REVISIONS:		(Date and Description)
--
--  INTERFACE:		HWND createInputLabel(HWND hwnd, LPARAM lParam, int x, int y, int x2, int y2, int id, char *s)
--
--  RETURNS:		a handle to the window
--
--	DESIGNERS:		Anthony Rachiero
--
--	PROGRAMMER:		Anthony Rachiero
--
--	NOTES: this is a wrapper function to create a label inside the oeprating window.
---------------------------------------------------------------------------------------*/
HWND createInputLabel(HWND hwnd, LPARAM lParam, int x, int y, int x2, int y2, int id, char *s)
{
	HWND inputwindow;
	char *ss = "STATIC";
	inputwindow = CreateWindow  (TEXT(ss),
					NULL,
					WS_VISIBLE | WS_BORDER | WS_CHILD,       
					x,              
					y,             
					x2,              
					y2,              
					hwnd,                       
					(HMENU)id,                      
					((LPCREATESTRUCT)lParam)->hInstance,                 
					NULL);	
	
	return inputwindow;
}
/*---------------------------------------------------------------------------------------
--	FUNCTION:		createInputField

--	DATE:			January 16, 2012
--
--	REVISIONS:		(Date and Description)
--
--  INTERFACE:		HANDLE createInputfield(HWND hwnd, LPARAM lParam, int x, int y, int x2, int y2, int id, char *s)
--
--  RETURNS:		a handle to the window
--
--	DESIGNERS:		Anthony Rachiero
--
--	PROGRAMMER:		Anthony Rachiero
--
--	NOTES: this is a wrapper function to create a text field within the operating window.
---------------------------------------------------------------------------------------*/
HANDLE createInputfield(HWND hwnd, LPARAM lParam, int x, int y, int x2, int y2, int id, char *s)
{
	HANDLE inputwindow;
	char *ss = "edit";
	inputwindow = CreateWindow  (TEXT(ss),
					TEXT (s),
					WS_VISIBLE | WS_BORDER | WS_CHILD,       
					x,              
					y,             
					x2,              
					y2,              
					hwnd,                       
					(HMENU)id,                      
					((LPCREATESTRUCT)lParam)->hInstance,                 
					NULL);	
	
	return inputwindow;
}
/*---------------------------------------------------------------------------------------
--	FUNCTION:		wsaInit()

--	DATE:			January 16, 2012
--
--	REVISIONS:		(Date and Description)
--
--  INTERFACE:		void wsaInit()
--
--  RETURNS:		void
--
--	DESIGNERS:		Anthony Rachiero
--
--	PROGRAMMER:		Anthony Rachiero
--
--	NOTES: this is a wrapper function to initiate WSA and WSAcleanup is an error occurs
			during initiation.
---------------------------------------------------------------------------------------*/
void wsaInit()
{
	int err = 0;
	WSADATA	 WSAData;
	WORD		 wVersionRequested;

	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup( wVersionRequested, &WSAData );

	if (err != 0)
	{
		WSACleanup();
		exit(18);;
	}
}