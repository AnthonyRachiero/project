#include "server.h"

/*---------------------------------------------------
-- source file: server.c - a server application that reads files
--			and sends them to a client application.
--
--
-- programmer: Anthony Rachiero
-- designer: Anthony Rachiero
--
-- notes: this program creates 2 message queues, the first 
-- 	queue reads a filename from a client program and fills
--	data structures holding buffer and priority type. it then 
--	takes the filename, opens the file and reads the contents
--	into a buffer. the second message queue sends the contents
--	of the file to the client as they get read. multiple client
--	may connect to the server and may read multiple files at a
--	time.
--
--
-----------------------------------------------------*/
int main(int argc, char *argv[])
{
	key_t			 mkey;
	int		 	 	 msg_qid;
	struct			 msqid_ds;
	struct 			 msq_status;
	sendmsg  		 msg;
	filedescriptor   fd;
	FILE 			*fp;
	int 			 msg_qid2;
	key_t 			 mkey2;
	int 			 priority;

	mkey = ftok(".", 'a');
	mkey2 = ftok(".", 'b');

	if((msg_qid = msgget(mkey, 0777 | IPC_CREAT | IPC_EXCL)) == -1)
	{
		perror("mssget fail");
		return -1;
	
	}
	if((msg_qid2 = msgget(mkey2, 0777 | IPC_CREAT | IPC_EXCL)) == -1)
	{
		perror("msgget fail");
		return -1;
	}
	while(1)
	{
		if((msgrcv(msg_qid, &fd, sizeof(fd) - sizeof(long), 1, 0)) == -1)
		{
			perror("msgrcv fail");
			return -2;
		}
		if(fork() == 0)
		{
			if((fp = fopen(fd.filename, "r")))
			{	
			
				msg.mtype = (long)fd.pid;		
				priority = fd.priority;	
				msg.mtext[255] = ' ';

				while(fgets(msg.mtext, priority, fp) != NULL)
				{
					if((msgsnd(msg_qid2, &msg, priority, 0)) == -1)
					{	
						printf("msgsnd fail");
						return -2;
					}
				}
				msg.mtext[255] = '\3';
				msgsnd(msg_qid2, &msg, sizeof(msg) - sizeof(long),0);
			}
		}
	}
	/*
	if((msgctl(msg_qid, IPC_RMID, 0)) == -1)
	{
		return -3;
	}
*/
	return 0;
}

