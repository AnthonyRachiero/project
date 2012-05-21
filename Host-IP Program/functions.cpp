#include "main.h"

char output[1000];
/*---------------------------------------------------------------------------------------
--	FUNCTION:		Port_ser

--	DATE:			January 16, 2012
--
--	REVISIONS:		(Date and Description)
--
--  INTERFACE:		int Port_ser(LPSTR input, LPSTR input2)
--
--  RETURNS:		int
--
--	DESIGNERS:		Anthony Rachiero
--
--	PROGRAMMER:		Anthony Rachiero
--
--	NOTES:This function takes a port number and a protocol as arguments and retrieves
--			the corresponding service name
---------------------------------------------------------------------------------------*/
int Port_ser(LPSTR input, LPSTR input2)
{
	struct servent *sv;
	int s_port;
	
	WORD wVersionRequested = MAKEWORD(2,2);   
	WSADATA wsaData;                         

	// Open up a Winsock v2.2 session
	WSAStartup(wVersionRequested, &wsaData);

	s_port = atoi(input);
		
	sv = getservbyport (htons(s_port), input2);
	if (sv == NULL)
	{
		strcpy(output, "Error in getservbyport\n");
		return 2;
	} 
	strcpy(output, sv->s_name);		

	WSACleanup();
	return 1; 
}
/*---------------------------------------------------------------------------------------
--	FUNCTION:		Ser_Port

--	DATE:			January 16, 2012
--
--	REVISIONS:		(Date and Description)
--
--  INTERFACE:		int Ser_Port(LPSTR input, LPSTR input2)
--
--  RETURNS:		int
--
--	DESIGNERS:		Anthony Rachiero
--
--	PROGRAMMER:		Anthony Rachiero
--
--	NOTES:This function takes a service name and a protocol and retrieves the 
--			correspoding port number
---------------------------------------------------------------------------------------*/
int Ser_Port(LPSTR input, LPSTR input2)
{
	struct servent *sv;
	int portout;

	WORD wVersionRequested = MAKEWORD(2,2);   
	WSADATA wsaData;                          

	// Open up a Winsock v2.2 session
	WSAStartup(wVersionRequested, &wsaData);

	sv = getservbyname (input, input2);
	if (sv == NULL)
	{
		strcpy(output, "Error in getservbyport\n");
		return 1;
	} 

	portout = ntohs(sv->s_port);
	
	sprintf(output, "%d", portout);
	WSACleanup();
	return 1;
}
/*---------------------------------------------------------------------------------------
--	FUNCTION:		IP_Hostt

--	DATE:			January 16, 2012
--
--	REVISIONS:		(Date and Description)
--
--  INTERFACE:		int IP_Hostt(LPSTR input)
--
--  RETURNS:		int
--
--	DESIGNERS:		Anthony Rachiero
--
--	PROGRAMMER:		Anthony Rachiero
--
--	NOTES: This function takes an IP address or a host name and retrieves the 
--			corresponding host/IP and displays it
---------------------------------------------------------------------------------------*/
int IP_Hostt(LPSTR input)
{

	int		a;
	struct	hostent *hp;
	struct	in_addr my_addr, *addr_p;
	char	**p;
	char	ip_address[256];      // String for IP address
					
	WORD wVersionRequested = MAKEWORD(2,2);   
	WSADATA wsaData;                          

	// Open up a Winsock v2.2 session
	WSAStartup(wVersionRequested, &wsaData);



	addr_p = (struct in_addr*)malloc(sizeof(struct in_addr));
	addr_p = &my_addr;
	
	if (isdigit(*input))	// Dotted IP?
	{
   		if ((a = inet_addr(input) == 0))
		{
			strcpy(output, "IP Address must be of the form x.x.x.x\n");
			return 1;
		}

		// Copy IP address  into ip_address
		strcpy(ip_address, input);
		addr_p->s_addr=inet_addr(ip_address) ;

				
		hp = gethostbyaddr((char *)addr_p, PF_INET, sizeof (my_addr));

		if (hp == NULL)
		{
			strcpy(output, "host information for %s not found\n");
			return 1;
		}
	}
	else
	{
   		if ((hp = gethostbyname (input)) == NULL)	// Host name?
		{
      		switch (h_errno)
			{
         		case HOST_NOT_FOUND:
					strcpy(output, "%s: No such host %s\n");
         			
					return 1;
         		case TRY_AGAIN:
					strcpy(output,"%s: host %s try again later\n");
         			 
					return 1;
         		case NO_RECOVERY:
					strcpy(output,  "%s: host %s DNS Error\n");
         			
         	 		return 1;
				case NO_ADDRESS:
					strcpy(output,  "%s: No IP Address for %s\n");
       
         	 		return 1;
				default:
					strcpy(output,  "Unknown Error:  %d\n");
         		
					return 1;
			}
		}
	}
	for (p = hp->h_addr_list; *p != 0; p++)
	{
   		struct in_addr in;
		char **q;

		memcpy(&in.s_addr, *p, sizeof (in.s_addr));

		if(isdigit(input[1]))
		{
			strcpy(output, hp->h_name);
		}
		else
		{
			strcpy(output,  inet_ntoa(in));
		}

		//for (q = hp->h_aliases; *q != 0; q++)
      	//	printf(" \t\t\t\t   Aliases: %s\n", *q);
	//	putchar('\n');
	}
   
	WSACleanup();
	return 1;
}