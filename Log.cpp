#include "AssistClsHeader.h"

std::fstream Log::m_EventLog;
char Log::m_Buff[1024]{};
char Log::m_TimeBuff[26]{};

Log::Log() {
	if (!FileExists()) {
		m_EventLog.open(logFile, std::ios::in | std::ios::out | std::ios::trunc);
		m_EventLog.close();
	}
}
bool Log::FileExists() {
	return std::ifstream(logFile).good();
}
char* Log::currentTime() {
	const time_t now = time(0);
	ctime_s(m_TimeBuff, sizeof(m_TimeBuff), &now);
	return m_TimeBuff;
}
void Log::logConnection(int id, bool con_OR_discon) {
	m_EventLog.open(logFile, std::ios::in | std::ios::out | std::ios::app);
	m_EventLog << "Client with session id " << id << (con_OR_discon? " connected" : " disconnected") << " at " << currentTime() << "\n";
	m_EventLog.close();
}
void Log::logMessaging(int msgType, int id, bool recv_OR_send) {
	std::string strMsgType;
	switch (msgType) {
	case 0: {
		strMsgType = "Data"; break;
	}
	case 1: {
		strMsgType = "Info"; break;
	}
	case 2: {
		strMsgType = "Error"; break;
	}
	default: break;
	}
	m_EventLog.open(logFile, std::ios::in | std::ios::out | std::ios::app);
	m_EventLog << strMsgType << " message was " << (recv_OR_send? "recovered by": "sended to")
		<< "a client with session id " << id << " at " << currentTime() << "\n";
	m_EventLog.close();
}
std::string Log::getLog() {
	m_EventLog.open(logFile, std::ios::in);
	std::stringstream ss;
	ss << m_EventLog.rdbuf();
	m_EventLog.close();
	return ss.str();
}