			terminal processing and replaces it 
			with proccesses that replicate the 
			function.

Functions: int translate(int pfd[2], int pfd1[2]);
           int output(int pfd[2], int pfd1[2]);
	   int input(int pfd[2], int pfd1[2]);
	   void openpipe(int pfd[2]);

Designer: Anthony Rachiero
Programmer: Anthony Rachiero

Notes: This program creates 3 proccesses to mimic a unix 
	terminal, it reads input from the keyboard and 
	echoes it to the screen, special keys replace the
	BACKSPACE,ENTER,TERMINATE,LINEKILL, and INTERRUPT
	functions.
------------------------------------------------------------*/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define BUFFSIZE 256
#define ENTER 'E'
#define TERMINATE 'T'
#define BACKSPACE 'X'
#define LINEKILL 'K'
#define INTERRUPT 11

int translate(int pfd[2], int pfd1[2]);
int output(int pfd[2], int pfd1[2]);
int input(int pfd[2], int pfd1[2]);
void openpipe(int pfd[2]);


/*--------------------------------------------------------------
Function: main

Designer: Anthony Rachiero
programmer: Anthony Rachiero

interface: int main(void)

returns: 0 - indicating all processes have been killed

NOTES: The main function creates the proccesses and the pipes
	with which the proccesses will communicate, it also
	kill every process when the input process is killed.
---------------------------------------------------------------*/
int main(void)
{
	pid_t childpid;
	pid_t childpid2;
	int input_output[2];
	int input_translate[2];
	int translate_output[2];

	system("stty raw -echo igncr");
	
	openpipe(input_output);
	openpipe(input_translate);
	openpipe(translate_output);

	childpid = fork();
	
	if(childpid == 1)
	{
		perror("fork failed!");
		return 1;
	}
	if(childpid == 0)
	{
		output(input_output, translate_output);
		kill(childpid, SIGKILL);
		return 1;
	}
	else
	{
		childpid2 = fork();
		if(childpid2 == 1)
		{
			perror("fork failed");
			return 1;
		}
		if(childpid2 == 0)
		{
			translate(translate_output, input_translate);
			kill(childpid2, SIGKILL);
			return 1;
		}
		else
		{
			input(input_output, input_translate);
			
//			kill(childpid, SIGKILL);
//			kill(childpid2, SIGKILL);
			system("stty cooked echo -igncr");
			return 0;
		}
	}
	return 0;
}
/*----------------------------------------------------------------
Function: openpipe

Designer: Anthony Rachiero
Programmer: Anthony Rachiero

Interface: void openpipe(int pfd[2]);

returns: void

NOTES: a simple wrapper to perform the function of opening pipes.
----------------------------------------------------------------*/
void openpipe(int pfd[2])
{
	if(pipe(pfd) < 0)
	{
		perror("pipe call");
		exit(1);
	}
}

/*----------------------------------------------------------------
Function: translate

Designer: Anthony Rachiero
Programmer: Anthony Rachiero

Interface: int translate(int pfd[2], int pfd1[2]);

returns: 0 when killed

NOTES: this function processes a buffer when the ENTER key is hit,
	it performs function like backspace and linekill, when it
	is done proccessing the buffer it writes it to the output
	process to be echoed to the screen.
----------------------------------------------------------------*/
int translate(int pfd[2], int pfd1[2])
{
	char user_input[BUFFSIZE];
	int nread;
	int count = 0;
	int i = 0;
	for(;;)
	{
		switch(nread = read(pfd1[0], user_input, BUFFSIZE))
		{
			case -1:
			case 0:
				printf("pipe empty");
				break;
			default:
				for(i = 0; i < nread; i++)
				{
					if(user_input[i] == TERMINATE)
					{
					        write(pfd[1], user_input, BUFFSIZE);	
						memset(user_input, '\0', BUFFSIZE);
						return 0;
					}
					if(user_input[i] == LINEKILL)
					{
						memset(user_input, '\0', BUFFSIZE);
					}
					if(user_input[i] == BACKSPACE)
					{
						user_input[i - 1] = ' ';
						user_input[i] = ' ';
					}
					if(user_input[i] == 'a')
					{
						user_input[i] = 'z';
					}
				}
				write(pfd[1], user_input, nread);
				memset(user_input, '\0', BUFFSIZE);
				i = 0;
				
			
				count = 0;
		}
	}

}


/*----------------------------------------------------------------
Function: output

Designer: Anthony Rachiero
Programmer: Anthony Rachiero

Interface: int output(int pfd[2], int pfd1[2]);

returns: 0 when killed

NOTES: this function reads characters from input or a buffer 
	from translate and echoes whatever it reads to the 
	screen, if it detects a TERMINATE character it kills 
	itself.
----------------------------------------------------------------*/
int output(int pfd[2], int pfd1[2])
{
	char user_input[BUFFSIZE] = "";
	char user_input2[1];
	int nread;
	int i = 0;
	while((nread = read(pfd[0], user_input2, 1)))
	{
		
		printf("%s", user_input2);
		fflush(stdout);
		//read the pipe for strings	
			if(user_input2[0] == TERMINATE)
			{
				read(pfd1[0], user_input, BUFFSIZE);
				printf("\r\n%s\n\r", user_input);
				return 0;
			}
			if(user_input2[0] == ENTER)
			{	
				read(pfd1[0], user_input, BUFFSIZE);
				printf("\r\n%s\r\n", user_input);
				memset(user_input, '\0', BUFFSIZE);
			}
		
		i = 0;
	}
	return 0;
}

/*----------------------------------------------------------------
Function: input 

Designer: Anthony Rachiero
Programmer: Anthony Rachiero

Interface: int input(int pfd [2], int pfd1[2]);

returns: 0 when killed

NOTES: this function gets characters from the keyboard, it will
	echo these characters to the screen, and depending on
	what characters it receives it will send the character
	to output to be echoed or it will send a buffer to 
	translate to be translated, it kills itself if TERMINATE
	is detected.
----------------------------------------------------------------*/
int input(int pfd[2], int pfd1[2])
{
	char user_input[BUFFSIZE], input;
	int char_count = 0;
	close(pfd[0]);
	close(pfd1[0]);
	
	for(;;)
	{
		input = getchar();
		write(pfd[1], &input, 1);
		/*
		if special enter key is hit then reset char_count
		*/
		if(input == ENTER || input == TERMINATE || input == INTERRUPT)
		{
			write(pfd1[1], user_input, char_count);
			//FIND OUT WHAT CNTRL-K IS 
			if(input == INTERRUPT)
			{
				return 0;
			}
			if(input == TERMINATE)
			{
				//FIND ALTERNATIVE TO SLEEP(), SLEEP WORKS HERE
				return 0;
			}
			char_count = 0;
			memset(user_input, '\0', BUFFSIZE);
			
		}
		else
		{	
			user_input[char_count++] = input;
		}
	}
	return 0;
}


