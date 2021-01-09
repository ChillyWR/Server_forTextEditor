#pragma once
#include "AssistClsHeader.h"

#pragma comment(lib, "ws2_32.lib")
//#include <WinSock2.h>
#include <WS2tcpip.h>


#include <vector>
#include <future>
#include <chrono>
#define locIP "127.0.0.1"

class ServerConnection : protected TextRedactor, Log {
protected:
	static constexpr int m_ClientAmount = 100;
	std::vector<std::future<void>> m_Futures;
	struct ConnectExceptions {
		static constexpr char SockLibNotFound[] = "Error: lib 'ws2_32.lib' not loaded";
		static constexpr char NoConnectionWClient[] = "Error: no connection with client";
		static constexpr char UnknownMsgType[] = "Error: unknown packet header";
		static constexpr char UnknownCommand[] = "Error: unknown command";
		static constexpr char RecoverError[] = "Error: failed to recover a message";
	};
	struct Commands {
		static constexpr char write[] = "write";
		static constexpr char output[] = "output";
		static constexpr char who[] = "who";
		static constexpr char disconnect[] = "disconnect";
	};
	static constexpr char succResult[] = "File successfully saved";
	enum class PacketHeader {
		DataTypeMsg,
		InfoTypeMsg,
		ErrorTypeMsg,
		CommandTypeMsg
	};
private:
	bool m_Active = false;
	sockaddr_in m_ServerAddr;
	sockaddr_in m_ClientAddr[m_ClientAmount]{};
	SOCKET m_ListenSock;
	SOCKET m_ClientSockets[m_ClientAmount]{};
	bool m_Connected[m_ClientAmount]{};
	struct m_MsgType {
		PacketHeader type;
		char* command;
		char* data;
		size_t size;
	};

	inline void libCheck();
	inline void addrSetter();
	inline void wiretapping();
	inline void connectWClient(size_t);
	inline void clientInfo(const size_t&);
	inline void defaultMessageSender(size_t);
	inline void messageSender(const size_t&, PacketHeader, std::string, std::string = "NULL");
	void respondWaiting(size_t);
	void messageRecover(const size_t&);
	m_MsgType& msgDescriptor(m_MsgType&, char*);
	void packetManeger(const size_t&, const m_MsgType&);
public:
	void svStartUp();
	void svShutDown();
	ServerConnection();
	~ServerConnection();
};
