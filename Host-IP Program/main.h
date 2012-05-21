#ifndef MAIN_H
#define MAIN_H
#include <windows.h>
#include "resource.h"
#include <stdio.h>
#include <string.h>


#define BUFFSIZE 1000

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;
int IP_Hostt(LPSTR input);
int Ser_Port(LPSTR input, LPSTR input2);
int Port_ser(LPSTR input, LPSTR input2);
#endif