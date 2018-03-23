#include <stdio.h>    
#include <winsock2.h>    
#include <Ws2tcpip.h>

#pragma comment(lib,"ws2_32.lib")    

static const int iPort = 8888;

int main(int argc, char* argv[])
{
	//initialization  
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return 0;
	}
   
	//create socket
	SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (slisten == INVALID_SOCKET)
	{
		printf("socket error !");
		return 0;
	}

	//bind    
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(iPort);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (::bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("bind error !");
	}

	//listen   
	if (listen(slisten, 5) == SOCKET_ERROR)
	{
		printf("listen error !");
		return 0;
	}

	//working loop
	SOCKET sClient;
	sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);
	char revData[512] = {0};
	char ipStr[20] = {0};
	while (true)
	{
		printf("waiting for connection...\n");
		sClient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
		if (sClient == INVALID_SOCKET)
		{
			printf("accept error !");
			continue;
		}

		inet_ntop(AF_INET, &remoteAddr.sin_addr, ipStr, 20);
		printf("Accept a connection: %s \r\n", ipStr);

		//receive   
		int ret = recv(sClient, revData, 512, 0);
		if (ret > 0)
		{
			revData[ret] = 0x00;
			printf("revData: %s", revData);
		}

		//send 
		const char * sendData = "Received data from client";
		send(sClient, sendData, strlen(sendData), 0);
		closesocket(sClient);
	}

	closesocket(slisten);
	WSACleanup();
	return 0;
}