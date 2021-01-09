#include "AssistClsHeader.h"

TextRedactor::TextRedactor() {
	if (!FileExists()) {
		m_Text.open(redactorFile, std::ios::in | std::ios::out | std::ios::trunc);
		m_Text << defaultText;
		m_Text.close();
	}
}

bool TextRedactor::FileExists() {
	return std::ifstream(redactorFile).good();
}

bool TextRedactor::checkText(char* newText) {
	char* text = new char[textSize + 1];
	text[textSize] = '\0';
	strcpy_s(text, textSize, newText);
	char* newToken = NULL;
	char* token = strtok_s(text, "\n", &newToken);
	int i = 0;
	for (; token != NULL; token = strtok_s(NULL, "\n", &newToken), i++) {
		if (!(sizeof(token) < 129))
			return false;
	}
	if (i > 8)
		return false;
	return true;
}

void TextRedactor::writeText(char* newText) {
	if (checkText(newText)) {
		m_Text.open(redactorFile, std::ios::in | std::ios::out | std::ios::trunc);
		m_Text << newText;
		m_Text.close();
	}
	else throw TextExceptions::InvalidChanges;
}

std::string TextRedactor::getText() {
	m_Text.open(redactorFile, std::ios::in);
	std::stringstream ss;
	ss << m_Text.rdbuf();
	m_Text.close();
	return ss.str();
}