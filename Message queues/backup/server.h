#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define BUFFSIZE 256
#define PRIORITY5 256
#define PRIORITY4 128
#define PRIORITY3 64
#define PRIORITY2 32
#define PRIORITY1 16

typedef struct
{
	long mtype;
	char mtext[BUFFSIZE];
}sendmsg;

typedef struct
{
	long mtype;
	int priority;
	pid_t pid;
	char filename[BUFFSIZE];
} filedescriptor;

typedef struct
{
	long mtype;
	char mtext[BUFFSIZE];
}receivemsg;
/*--------------------------------------------
-- function: getpriority
--
-- designer: Anthony Rachiero
-- programmer: Anthony Rachiero
--
-- interface: int getpriority(int priority)
		priority - msg priority set by the user
--
-- return: returns buffer read size 
--	   depending on priority entered
--
-- notes: This function takes the priority number
--	  from the user and traslates it to the 
--	  appropriate buffer size to be read by
--  	  that specific client.
--
---------------------------------------------*/
int getpriority(int priority);
