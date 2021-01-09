#include "ServerConnection.h"

std::string who = "Oleksii Kytsenkyi\tVariat 1\nClient-Server text redactor";

void ServerConnection::libCheck() {
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0)
		throw ConnectExceptions::SockLibNotFound;
}
void ServerConnection::addrSetter() {
	m_ServerAddr.sin_family = AF_INET;
	m_ServerAddr.sin_port = htons(2026);
	inet_pton(AF_INET, locIP, &m_ServerAddr.sin_addr.s_addr);
}
void ServerConnection::wiretapping() {
	m_ListenSock = socket(AF_INET, SOCK_STREAM, 0);
	bind(m_ListenSock, (sockaddr*)&m_ServerAddr, sizeof(m_ServerAddr));
	listen(m_ListenSock, SOMAXCONN);
}
void ServerConnection::connectWClient(size_t currentSockIndex) {
	SOCKET clientSock;
	int ClientAddr_Size = sizeof(m_ClientAddr[currentSockIndex]);
	if ((clientSock = accept(m_ListenSock, (SOCKADDR*)&m_ClientAddr, &ClientAddr_Size)) == INVALID_SOCKET)
		throw ConnectExceptions::NoConnectionWClient;
	m_Connected[currentSockIndex] = true;
	m_ClientSockets[currentSockIndex] = clientSock;
	clientInfo(currentSockIndex);
	Log::logConnection(currentSockIndex, true);
}
void ServerConnection::clientInfo(const size_t& currentSockIndex) {
	printf("Client connected with session id %u\n", currentSockIndex);
	char host[NI_MAXHOST]; memset(host, 0, NI_MAXHOST);
	char service[NI_MAXSERV]; memset(service, 0, NI_MAXSERV);
	if (getnameinfo((sockaddr*)&m_ClientAddr[currentSockIndex], sizeof(m_ClientAddr[currentSockIndex]), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
		printf("Host %s\tPort %s\n", host, service);
	else {
		inet_ntop(AF_INET, &m_ClientAddr[currentSockIndex].sin_addr, host, NI_MAXHOST);
		printf("Host: %s\tPort: %u\n", host, ntohs(m_ClientAddr[currentSockIndex].sin_port));
	}
}
void ServerConnection::svStartUp() {
	m_Active = true;
	printf("Waiting for connection\n");
	wiretapping();
	for (size_t sockIndex = 0; m_Active && sockIndex < UINT32_MAX; sockIndex++) {
		connectWClient(sockIndex);
		defaultMessageSender(sockIndex);
		m_Futures.push_back(
			std::async(std::launch::async, &ServerConnection::respondWaiting, this, sockIndex));
	}
}
inline void ServerConnection::defaultMessageSender(size_t currentSockIndex) {
	messageSender(currentSockIndex, PacketHeader::DataTypeMsg, ServerConnection::Commands::write, TextRedactor::getText());
}
void ServerConnection::messageSender(const size_t& currentSockIndex, PacketHeader msgType, std::string command, std::string data) {
	std::string msg = std::to_string((int)msgType) + " " + command + " " + data;
	int msg_size = msg.size();
	send(m_ClientSockets[currentSockIndex], (char*)&msg_size, sizeof(int), 0);
	send(m_ClientSockets[currentSockIndex], msg.c_str(), msg_size, 0);
	Log::logMessaging((int)msgType, currentSockIndex, false);
}
void ServerConnection::respondWaiting(size_t currentSockIndex) {
	while (m_Connected[currentSockIndex] && m_Active) messageRecover(currentSockIndex);
}
void ServerConnection::messageRecover(const size_t& currentSockIndex) {
	m_MsgType msgInst{};
	int recived = recv(m_ClientSockets[currentSockIndex], (char*)&msgInst.size, sizeof(size_t), NULL);
	if (recived == SOCKET_ERROR) {
		closesocket(m_ClientSockets[currentSockIndex]);
		throw ConnectExceptions::RecoverError;
	}
	else if (recived == 0) {
		printf("Client with session id %u disconnected", currentSockIndex);
		closesocket(m_ClientSockets[currentSockIndex]);
		m_Connected[currentSockIndex] = false;
		Log::logMessaging((int)msgInst.type, currentSockIndex, false);
	}
	else {
		char* msg = new char[msgInst.size + 1];
		msg[msgInst.size] = '\0';
		recived = recv(m_ClientSockets[currentSockIndex], msg, msgInst.size, NULL);
		if (recived == SOCKET_ERROR)
			throw ConnectExceptions::RecoverError;
		else if (recived == 0) {
			printf("Client with session id %u disconnected", currentSockIndex);
			closesocket(m_ClientSockets[currentSockIndex]);
			m_Connected[currentSockIndex] = false;
			Log::logMessaging((int)msgInst.type, currentSockIndex, false);
		}
		else
			packetManeger(currentSockIndex, msgDescriptor(msgInst, msg));
		delete[] msg;
	}
}
ServerConnection::m_MsgType& ServerConnection::msgDescriptor(m_MsgType& msgInst, char* msg) {
	char* newToken = NULL;
	char* token = strtok_s(msg, " ", &newToken);
	std::stringstream ss; ss << token;
	int msgType; ss >> msgType;
	msgInst.type = (ServerConnection::PacketHeader)msgType;
	token = strtok_s(NULL, " ", &newToken);
	msgInst.command = token;
	token = strtok_s(NULL, "", &newToken);
	msgInst.data = token;
	return msgInst;
}
void ServerConnection::packetManeger(const size_t& currentSockIndex, const m_MsgType& msgInst) {
	if (msgInst.type == PacketHeader::DataTypeMsg) {
		if (strcmp(msgInst.command, ServerConnection::Commands::write) == 0) {
			if (TextRedactor::checkText(msgInst.data)) {
				TextRedactor::writeText(msgInst.data);
				messageSender(currentSockIndex, PacketHeader::InfoTypeMsg, ServerConnection::Commands::output, ServerConnection::succResult);
			}
			else messageSender(currentSockIndex, PacketHeader::ErrorTypeMsg, ServerConnection::Commands::output, TextRedactor::TextExceptions::InvalidChanges);
		}
		else messageSender(currentSockIndex, PacketHeader::ErrorTypeMsg, ServerConnection::Commands::output, ConnectExceptions::UnknownCommand);
	}
	else if (msgInst.type == PacketHeader::CommandTypeMsg) {
		if (strcmp(msgInst.command, ServerConnection::Commands::who) == 0)
			messageSender(currentSockIndex, PacketHeader::InfoTypeMsg, ServerConnection::Commands::output, who);

		else if (strcmp(msgInst.command, ServerConnection::Commands::disconnect) == 0) {
			closesocket(m_ClientSockets[currentSockIndex]);
			m_Connected[currentSockIndex] = false;
		}
	}
	else messageSender(currentSockIndex, PacketHeader::ErrorTypeMsg, ServerConnection::Commands::output, ConnectExceptions::UnknownMsgType);
}

void ServerConnection::svShutDown() {
	m_Active = false;
	for (size_t i = 0; i < m_ClientAmount; i++) {
		if (m_Connected[i] == true)
			closesocket(m_ClientSockets[i]);
	}
	closesocket(m_ListenSock);
	WSACleanup();
}
ServerConnection::ServerConnection() {
	Log log; TextRedactor redactor;
	libCheck();
	addrSetter();
}
ServerConnection::~ServerConnection() {
	svShutDown();
}