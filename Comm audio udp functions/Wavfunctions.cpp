#include "Wavfunctions.h"
#include "UDPfunctions.h"



void playwav(UDPinfo *UI, TCPinfo *TI)
{
	FILE *fp;
	wavheader WH;
	char *data1;
	char data[BUFSIZE];
	int numpackets = 0;
	int bytestoSend = 0;
	int bufpos = 0;
	HWAVEOUT hWaveOut;
	WAVEFORMATEX wfx;
	int i = 0;
	int err = 0;
	int bytesent = 0;
	HANDLE hFile;
	DWORD readBytes;
	WSABUF filecontents;
	DWORD flags = 0;
	int byteSent = 0;
	time_t t1;


	filecontents.buf = (char *) malloc (BUFSIZE);
	filecontents.len = BUFSIZE;



	fp = wavOpen(&WH);
	data1 = (char *) malloc (BUFSIZE);


	if((hFile = CreateFile("Pokemons.wav",GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL)) == INVALID_HANDLE_VALUE) 
	{
		fprintf(stderr, "%s: unable to open file '%s'\n");
		ExitProcess(1);
	}

	TCPcontrolsend(TI, UI, WH.size);

	while(1) 
	{
		
		if(!ReadFile(hFile, data, sizeof(data), &readBytes, NULL))
		{
			break;
		}
		if(readBytes == 0)
		{
			break;
		}
		if(readBytes < sizeof(data)) 
		{
			printf("at end of buffer\n");
			memset(data + readBytes, 0, sizeof(data) - readBytes);
			printf("after memcpy\n");
		}
		memcpy(filecontents.buf, data, BUFSIZE);
		Sleep(5);
		sendto(UI->sd, filecontents.buf, BUFSIZE, flags, (struct sockaddr *)&UI->server, UI->server_len); 
	}
}


WAVEHDR* allocateBlocks(int size, int count)
{
	unsigned char* buffer;
	int i;
	WAVEHDR* blocks;
	DWORD totalBufferSize = (size + sizeof(WAVEHDR)) * count;
	/*
	 * allocate memory for the entire set in one go
	 */
	if((buffer = (unsigned char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, totalBufferSize)) == NULL) 
	{
		fprintf(stderr, "Memory allocation error\n");
		ExitProcess(1);
	}
	/*
	 * and set up the pointers to each bit
	 */
	blocks = (WAVEHDR*)buffer;
	buffer += sizeof(WAVEHDR) * count;
	for(i = 0; i < count; i++) 
	{
		blocks[i].dwBufferLength = size;
		blocks[i].lpData = (LPSTR)buffer;
		buffer += size;
	}
	return blocks;
}

void freeBlocks(WAVEHDR* blockArray)
{
	/* 
	 * and this is why allocateBlocks works the way it does
	 */ 
	HeapFree(GetProcessHeap(), 0, blockArray);
}

void writeAudio(HWAVEOUT hWaveOut, LPSTR data, int size ,WAVEHDR *waveBlocks, CRITICAL_SECTION waveCriticalSection)
{
	WAVEHDR* current;
	int remain = 0;
	//memset(&current, 0, sizeof(WAVEHDR));
	current = &waveBlocks[waveCurrentBlock];

	

	while(size > 0)
	{
	/* 
	 * first make sure the header we're going to use is unprepared
	 */
	if(current->dwFlags & WHDR_PREPARED)
	{
		waveOutUnprepareHeader(hWaveOut, current, sizeof(WAVEHDR));
	}
	if(size < (int)(BLOCK_SIZE - current->dwUser)) 
	{
	//	current->lpData = data;
		memcpy(current->lpData + current->dwUser, data, size);
		current->dwUser += size;
		break;
	}	
	remain = BLOCK_SIZE - current->dwUser;
	//current->lpData = (char *) malloc (BLOCK_SIZE);
	//current->lpData = data;
	memcpy(current->lpData + current->dwUser, data, remain);
	size -= remain;
	data += remain;
	current->dwBufferLength = BLOCK_SIZE;
	waveOutPrepareHeader(hWaveOut, current, sizeof(WAVEHDR));

	waveOutWrite(hWaveOut, current, sizeof(WAVEHDR));
	EnterCriticalSection(&waveCriticalSection);
	waveFreeBlockCount--;
	LeaveCriticalSection(&waveCriticalSection);
	/*
	 * wait for a block to become free
	 */
	while(!waveFreeBlockCount)
	{
		Sleep(10);
	}
	/*
	 * point to the next block
	 */
	waveCurrentBlock++;
	waveCurrentBlock %= BLOCK_COUNT;
	current = &waveBlocks[waveCurrentBlock];
	current->dwUser = 0;
	
	
	}

}
