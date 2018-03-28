#include "client_base.h"

#pragma comment(lib, "ws2_32.lib") 
//using namespace Net;

bool skt::Init()
{
	WORD socketVer = MAKEWORD(2, 2);
	WSADATA wsaData;
	return WSAStartup(socketVer, &wsaData) == 0;
}

void skt::UnInit()
{
	WSACleanup();
}

skt::Socket::Socket() :Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
{

}

skt::Socket::Socket(int af, int type, int proto)
{
	m_socket = socket(af, type, proto);
	if (m_socket == INVALID_SOCKET) {
		m_state = SocketState::SOCKET_INVALID;
	} else {
		m_state = SocketState::SOCKET_READY;
	}
}

skt::Socket::~Socket()
{
	closesocket(m_socket);
}

