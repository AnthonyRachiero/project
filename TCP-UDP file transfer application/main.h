#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "UDPclient.h"
#include "UDPserver.h"
#include "TCPclient.h"
#include "TCPserver.h"
#include "resource1.h"

#define BUFFSIZE 256
#define PACKETSIZE1 1024
#define PACKETSIZE2 4096
#define PACKETSIZE3 20480
#define PACKETSIZE4 61440

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;
HANDLE createInputbutton(HWND hwnd, LPARAM lParam, int x, int y, int x2, int y2);
HANDLE createInputfield(HWND hwnd, LPARAM lParam, int x, int y, int x2, int y2, int id, char *s);
HWND createInputLabel(HWND hwnd, LPARAM lParam, int x, int y, int x2, int y2, int id, char *s);
int client(int protocol, TCPclientInfo *TCI, UDPclientInfo *UCI, controlTCPclientInfo *UTCI, HWND hwnd);
int server(int protocol, TCPserverInfo TSI, UDPserverInfo USI, controlTCPserverInfo UTSI, UDPthreadinfo UTI, HWND hwnd);
int sendfile(int protocol, TCPclientInfo TCI, UDPclientInfo UCI, controlTCPclientInfo UTCI, HWND hwnd);
void wsaInit();
