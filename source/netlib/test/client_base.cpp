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

int skt::Socket::Connect(sockaddr * pAddrStrc, const int iSize)
{
	auto ret = ::connect(m_socket, pAddrStrc, iSize);
	if (ret == SOCKET_ERROR){
		m_state = SOCKET_INVALID;
	} else {
		m_state = SOCKET_CONNECTED;
	}
	return ret;
}

int skt::Socket::Send(const char* pData, const int iSize, const int iFlag/* default: 0*/)
{
	return ::send(m_socket, pData, iSize, iFlag);
}

skt::SocketState skt::Socket::GetState()
{
	return m_state;
}

