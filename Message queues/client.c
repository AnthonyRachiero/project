#include "server.h"
#include <pthread.h>
/*-----------------------------------------------------
-- source file: client.c - a client application that sends a file
			name to a server application and reads back
			the file contents
--
--
-- programmer: Anthony Rachiero
-- Designer: Anthony Rachiero
--
--
--
-- notes: This program first opens an existing message queue
--		created by a server, it then sends a filename and 
-- 		a priority number to the server to be processed.
--		it then opens another message queue to begin reading
--		the contents of the file correspoding to the filename
--		passed to the server, it then prints the contents
--		and exits when end of file is met. 
--
--
-------------------------------------------------------*/
int main(int argc, char *argv[])
{
	pthread_t thread1;
	int				 msg_qid;
	key_t			 mkey;
	key_t			 mkey2;
	receivemsg 		 msg;
	filedescriptor 	 fd;
	int 			 msg_qid2;
	pid_t 			 pid;
	int 			 priority = 0;
	threadinfo 		 t1;

	mkey = ftok(".", 'a');
	mkey2 = ftok(".", 'b');

	if((msg_qid = msgget(mkey, 0)) == - 1)
	{
		perror("msgget fail");
		return -1;
	}

	strcpy(fd.filename, argv[1]);
	priority = atol(argv[2]);
	fd.pid = getpid();
	fd.mtype = 1;

	priority = getpriority(priority);

	fd.priority = priority;	

	if((msgsnd(msg_qid, &fd, sizeof(fd) - sizeof(long), 0)) == -1)
	{
		perror("msgsnd fail");
		return -2;
	}
	
	if((msg_qid2 = msgget(mkey2, 0)) == -1)
	{
		perror("msgget fail");
		return -1;
	}
	
	pid = getpid();
	priority = fd.priority;

	t1.priority = priority;
	t1.msg_qid2 = msg_qid2;
	t1.pid = pid;
	t1.mtext = mtext;

	pthread_create(&thread1, NULL, readfile, &t1);
	pthread_join(thread1, NULL);
	return 0;
}
int getpriority(int priority)
{
	switch(priority)
	{
		case 1:
			priority = 	PRIORITY1;
			break;	
		case 2:
			priority = 	PRIORITY2;
			break;	
		case 3:
			priority = 	PRIORITY3;
			break;	
		case 4:
			priority = 	PRIORITY4;
			break;	
		case 5:
			priority = 	PRIORITY5;
			break;	
	}
	return priority;
}
void* readfile(void *p)
{

	while(1)
	{	
		if((msgrcv(p.msg_qid2, p.msg, p.priority, p.pid, 0)) == -1)
		{
			printf("Finished");
			return -2;
		}
		if(p.mtext[255] == '\3')
		{
			perror("exiting");
			exit(0);
		}
		printf("%s" , p->msg.mtext);
		fflush(stdout);
	}
}
