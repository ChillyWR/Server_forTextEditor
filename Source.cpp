#include "ServerConnection.h"

int main(int argc, char* argv[]) {
	try {
		ServerConnection server;
		server.svStartUp();
	}
	catch (const char* err) {
		printf("%s\n", err);
	}
	catch (...) {
		printf("Unknown error\n");
	}
	return 0;
}