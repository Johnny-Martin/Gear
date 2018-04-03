#pragma once
#include "client_base.h"

namespace skt {
namespace client {
	class BlockingSocket{
	public:
		BlockingSocket(skt::Socket& socket);
		int Connect(const char* cszIP, const int iPort);
		bool Send(const char* pData, const int iSize);
	private:
		skt::Socket m_socket;
	};

	class TestBlockingSocket : public test::ITestUnit {
		virtual void DoTest();
	};
}
}