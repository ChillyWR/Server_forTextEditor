#pragma once
#include <string>
#include <sstream>
#include <fstream>
#include <ctime>

class TextRedactor {
protected:
	static constexpr char redactorFile[74] = "redactor.txt";
	static constexpr int textRowsChar[2] = { 8, 128 };            //{ rows amount; characters in each row}
	static constexpr int textSize = textRowsChar[0] * textRowsChar[1];
	static constexpr char defaultText[textSize] = {
	   "Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n"
	   "Mauris venenatis lobortis elementum.Nam ac nunc bibendum erat congue varius id ac purus.\n"
	   "Vivamus ut mi semper, ultrices nibh ac, gravida sapien.Nunc convallis mattis sagittis.\n"
	   "Fusce venenatis vulputate est a facilisis.Vivamus rhoncus laoreet euismod.\n"
	   "Sed pulvinar velit ante, sed suscipit lectus convallis quis.\n"
	   "Ut sit amet quam vitae purus faucibus facilisis quis eget eros.Proin et hendrerit elit.\n"
	   "Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia curae;\n"
	   "Aenean rutrum, metus quis ullamcorper pharetra, velit ante pretium turpis, ac sagittis nisi velit sed leo.\n"
	};
	struct TextExceptions {
		static constexpr char InvalidChanges[] = "Error: invalid changes";
	};
private:
	std::fstream m_Text;
	char m_Buff[textSize]{};
	static inline bool FileExists();
public:
	TextRedactor();
	static bool checkText(char*);
	void writeText(char*);
	std::string getText();
};

class Log {
protected:
	static constexpr char logFile[8] = "log.txt";
private:
	static std::fstream m_EventLog;
	static char m_Buff[1024];
	static char m_TimeBuff[26];

	static inline bool FileExists();
	static inline char* currentTime();
public:
	Log();
	void logConnection(int, bool);
	void logMessaging(int, int, bool);
	std::string getLog();
};
