#pragma once
#include "client_base.h"

namespace skt {
namespace client {
	class BlockingSocket{
	public:
		BlockingSocket(skt::Socket& socket);
		int connect(const char* cszIP, const int iPort);
		int connect(sockaddr * pAddrStrc, const int iSize);

	private:
		skt::Socket socket;
	};

	class TestBlockingSocket : public test::ITestUnit {
		virtual void DoTest();
	};
}
}