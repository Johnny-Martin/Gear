#pragma once
#include "client_blocking.h"
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib") 

skt::client::BlockingSocket::BlockingSocket(skt::Socket& _socket)
	:m_socket(_socket)
{
}

int skt::client::BlockingSocket::Connect(const char* cszIP, const int iPort)
{
	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(iPort);
	inet_pton(AF_INET, cszIP, &serAddr.sin_addr.S_un.S_addr);

	return m_socket.Connect((sockaddr *)&serAddr, sizeof(serAddr));
}

bool skt::client::BlockingSocket::Send(const char* pData, const int iSize)
{
	if (m_socket.GetState() != SOCKET_CONNECTED){
		return false;
	}

	m_socket.Send(pData, iSize, 0);
	//return true;
}

