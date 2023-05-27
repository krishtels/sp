#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<stdio.h>
#include<iostream>
#include<winsock2.h>

#pragma comment (lib, "ws2_32.lib")

int main() {
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) { return 0; }

	while (true) {
		SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
		// указание адреса и порта сервера
		sockaddr_in dest_addr;
		dest_addr.sin_family = AF_INET;
		dest_addr.sin_port = htons(1280);
		dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		connect(s, (sockaddr*)&dest_addr, sizeof(dest_addr));

		char buf[100];

		std::cout << "Enter the string:" << std::endl;
		fgets(buf, sizeof(buf), stdin);
		send(s, buf, 100, 0);

		if (recv(s, buf, sizeof(buf), 0) != 0) {
			std::cout << "Poluchenaya stroka:" << std::endl << buf << std::endl;
		}
		closesocket(s);
	}
	WSACleanup();
}