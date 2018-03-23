
#include<WINSOCK2.H>  
#include<STDIO.H>  
#include<iostream>  
#include<string>  
#include <Ws2tcpip.h>

using namespace std;
#pragma comment(lib, "ws2_32.lib")  

static const int iPort  = 8888;
static char* strIP		= "127.0.0.1";

int main()
{
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
		return 0;
	}
	while (true) {
		SOCKET sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sclient == INVALID_SOCKET)
		{
			printf("invalid socket!");
			return 0;
		}

		sockaddr_in serAddr;
		serAddr.sin_family = AF_INET;
		serAddr.sin_port = htons(iPort);
		inet_pton(AF_INET, strIP, &serAddr.sin_addr.S_un.S_addr);
		if (connect(sclient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
		{ 
			printf("connect error !");
			closesocket(sclient);
			return 0;
		}

		string data;
		cin >> data;
		const char * sendData;
		sendData = data.c_str();   //string转const char*   
								   //char * sendData = "你好，TCP服务端，我是客户端\n";  
		send(sclient, sendData, strlen(sendData), 0);

		char recData[255];
		int ret = recv(sclient, recData, 255, 0);
		if (ret > 0) {
			recData[ret] = 0x00;
			printf(recData);
		}
		closesocket(sclient);
	}


	WSACleanup();
	return 0;

}
