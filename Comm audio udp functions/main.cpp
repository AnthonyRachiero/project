#include "UDPfunctions.h"

int main(int argc, char *argv[])
{
	UDPinfo UI;
	TCPinfo TI;
	UDPinfo Userver;
	TCPinfo control;
	int err = 0;
	WSADATA	 WSAData;
	WORD		 wVersionRequested;
	int BytesToRead = 0;
	int BytesRecv = 0;

	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup( wVersionRequested, &WSAData );
	
	UI.port = atoi(argv[1]);
	UI.hostname = argv[2];
	Userver.port = atoi(argv[1]);
	Userver.hostname = argv[2];
	
	if(atoi(argv[3]) == 1)
	{
		CreateSocket(&UI.sd, 1);
		CreateSocket(&TI.sd, 2);
		StoreServerInfo(&TI, &UI, 1);

		GetHost(&TI, &UI);

		CopyAddress(&UI, &TI);

		TCPconnect(&TI);
		playwav(&UI, &TI);
	}
	else
	{
	
		CreateSocket(&Userver.sd, 1);
		CreateSocket(&control.sd, 2);

		StoreServerInfo(&control, &Userver, 2);

		BindSockets(&Userver, &control);

		ListenForConnections(&control, 10);
		TCPaccept(&control);
		BytesToRead = TCPread(&control);
	
		fprintf(stdout,"playing song");
		UDPread(&Userver, BytesToRead);
		
	}

	return 0;
}


