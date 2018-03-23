#pragma once

#include "..\stdafx.h"
#include "WinSock2.h"

namespace Net {
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
		Socket();
		Socket(int af, int type, int proto);
		~Socket();

	private:
		SOCKET		m_socket;
		SocketState m_state;
	};

	class HTTPClient {

	};

	class HTTPSClient {

	};

	class WebSocket {

	};

	class NetTask {

	};

	class NetLib {

	};
}
