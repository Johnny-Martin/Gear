#pragma once
#include "WinSock2.h"

namespace skt {
	enum SocketState {
		SOCKET_UNDEFIINE = 0,
		SOCKET_READY,
		SOCKET_INVALID,
		SOCKET_CONNECTED,
		SOCKET_RECEIVING,
		SOCKET_RECEIVED,
		SOCKET_CLOSED
	};

	static bool Init();
	static void UnInit();

	class Socket {
	public:
		Socket();
		Socket(int af, int type, int proto);
		~Socket();
		int Connect(sockaddr * pAddrStrc, const int iSize);
		int Send(const char* pData, const int iSize, const int iFlag = 0);
		SocketState GetState();
	private:
		SOCKET		m_socket;
		SocketState m_state;
	};
}

namespace test {
	class ITestUnit
	{
	public:
		virtual void DoTest(void) = 0;

	};
}