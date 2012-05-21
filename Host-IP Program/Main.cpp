/*---------------------------------------------------------------------------------------
--	SOURCE FILE:	Main.cpp 
--
--	PROGRAM:		4985Ass1.exe
--
--	FUNCTIONS:		
--
--	DATE:			January 5, 2008
--
--	DESIGNERS:		Anthony Rachiero
--
--	PROGRAMMER:		Anthony Rachiero
--
--	NOTES: This program finds hostnames/IP's from given hostnames/IP's, it also finds
--		   service names and port numbers depending on which data is fed to the program.
--		   The program determines what to fetch based on the user input which is retrieved
--		   by using text boxes and menu items.
---------------------------------------------------------------------------------------*/
#include "main.h"

extern char output[1000];


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
                          TEXT ("Assignment 1"), // window caption
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
/*---------------------------------------------------------------------------------------
--	FUNCTION: WndProc 

--	DATE:			January 16, 2012
--
--	REVISIONS:		(Date and Description)
--
--  INTERFACE:		LRESULT CALLBACK WndProc (HWND hwnd, UINT message, 
												WPARAM wParam, LPARAM lParam)
--
--  RETURNS:		Nothing
--
--	DESIGNERS:		Anthony Rachiero
--
--	PROGRAMMER:		Anthony Rachiero
--
--	NOTES:
--     This simple  program  will  take a user specified port number nd protocol
--     and resolve it into its service name. 
--	   Remember to add "WS2_32.Lib" to the project source list.
---------------------------------------------------------------------------------------*/
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
     HDC         hdc ;
     PAINTSTRUCT ps ;
     RECT        rect ;
	 HMENU		 hmenu ;
	 static HANDLE hbox, hbutton, hlabel, HportButton;
	 HWND hinput, houtput, hinput2;
	 char input[BUFFSIZE];
	 char input2[BUFFSIZE];
	 static int conversionType = 40001;
	 int num = 0;


     switch (message)
     {
     case WM_CREATE:

		  hdc = GetDC(hwnd);
		  hmenu = LoadMenu(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDR_MENU1));
          SetMenu(hwnd, hmenu);


		  hbox = CreateWindow  (TEXT ("edit"),
								TEXT (""),
                                WS_VISIBLE | WS_BORDER | WS_CHILD,       
							    50,              
								100,             
								200,              
								30,              
								hwnd,                       
								(HMENU)ID_TEXTBOX,                      
								((LPCREATESTRUCT)lParam)->hInstance,                 
								NULL);	

		  HportButton = CreateWindow  (TEXT ("edit"),
								TEXT (""),
                                WS_VISIBLE | WS_BORDER | WS_CHILD,       
							    50,              
								130,             
								200,              
								30,              
								hwnd,                       
								(HMENU)ID_HPORT,                      
								((LPCREATESTRUCT)lParam)->hInstance,                 
								NULL);	

		  hbutton = CreateWindow  (TEXT ("button"),
								TEXT ("Resolve"),
                                WS_VISIBLE | WS_BORDER | WS_CHILD,       
								50,              
								40,             
								140,              
								20,              
								hwnd,                       
								(HMENU)ID_BUTTON,                      
								((LPCREATESTRUCT)lParam)->hInstance,                 
								NULL);	

		  hlabel = CreateWindow  (TEXT ("STATIC"),
								TEXT (""),
                                WS_VISIBLE | WS_BORDER | WS_CHILD,       
								50,              
								160,             
								200,              
								30,              
								hwnd,                       
								(HMENU)ID_LABEL,                      
								((LPCREATESTRUCT)lParam)->hInstance,                 
								NULL);	
		  return 0 ;
	 case WM_COMMAND:
		 hdc = GetDC(hwnd);
		  TextOut(hdc,260,100,"<-- in this box enter Hostname, IP, service name, or Port Number", 70);
		  TextOut(hdc,260,130, "<-- in this box enter Protocol when needed", 43);
		 switch(LOWORD(wParam))
		 {
			case ID_OPTIONS_HOST:
				conversionType = ID_OPTIONS_HOST;
				break;
			case ID_OPTIONS_IP:
				conversionType = ID_OPTIONS_IP;
				break;
			case ID_OPTIONS_PROTO:
				conversionType = ID_OPTIONS_PROTO;
				break;
			case ID_OPTIONS_PORTNUM:
				
					conversionType = ID_OPTIONS_PORTNUM;
				
				break;
			case ID_BUTTON:
				//input = (LPTSTR)malloc(sizeof(char));
				hinput = GetDlgItem(hwnd, ID_TEXTBOX);
				GetWindowText(hinput, input, 50);
				houtput = GetDlgItem(hwnd, ID_LABEL);
				
				hinput2 = GetDlgItem(hwnd, ID_HPORT);
				GetWindowText(hinput2, input2, 50); 

				switch(conversionType)
				{
					case ID_OPTIONS_HOST:
						IP_Hostt(input);
						SetWindowText(houtput, output);
						break;
					case ID_OPTIONS_IP:
						IP_Hostt(input);
						SetWindowText(houtput, output);
						break;
					case ID_OPTIONS_PROTO:

						//call function
						Ser_Port(input, input2);

						SetWindowText(houtput, output);
						break;
						
					case ID_OPTIONS_PORTNUM:			
						//hinput2 = GetDlgItem(hwnd, ID_HPORT);
						//GetWindowText(hinput2, input2, 50);

						//call function
						Port_ser(input, input2);
						SetWindowText(houtput, output);
						
						break;
						
				}
				
		 }


		 return 0;
     case WM_PAINT:
			hdc = GetDC(hwnd);
			TextOut(hdc,260,100,"<-- in this box enter Hostname, IP, service name, or Port Number", 70);
			TextOut(hdc,260,130, "<-- in this box enter Protocol when needed", 43);
          return 0 ;
          
     case WM_DESTROY:
          PostQuitMessage (0) ;
          return 0 ;
     }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
}


