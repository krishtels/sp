#include<stdio.h>
#include<iostream>
#include<winsock2.h>
#include<algorithm>
#include <string>

#pragma comment (lib, "ws2_32.lib")

DWORD WINAPI ThreadFunc(LPVOID client_socket) {
	SOCKET s2 = ((SOCKET*)client_socket)[0];
	char buf[100];
	//send(s2,"Welcome new client!\n",sizeof("Welcome new client!\n"),0);
	while (recv(s2, buf, sizeof(buf), 0)) {
		double a = atof(buf);
		int i = 0;

		for (;i<strlen(buf); i++)
		{
			if (buf[i] == ' ')
				break;
		}
		double b = atof(buf + i++);
		for (; i < strlen(buf); i++)
		{
			if (buf[i] == ' ')
				break;
		}
		double c = atof(buf + i++);
		
		std::string ans;
		double m = max(a, max( b, c));
		if ((m < a + b + c - m) && a > 0 && b > 0 && c > 0)
		{
			double P, S;
			ans = "YES   S=";
			P = (a + b + c) / 2.0;
			S = sqrt(P * (P - a) * (P - b) * (P - c));
			ans += std::to_string(S);
		}
		else
			ans = "NO";
		std::cout << ans << std::endl;
		strcpy_s(buf, ans.c_str());
		send(s2, buf, 100, 0);
	}
	closesocket(s2);
	return 0;
}

int numcl = 0;

void print() {
	if (numcl) printf("%d client connected\n", numcl);
	else printf("No clients connected\n");
}

int main() {
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) { return 0; }
	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(1280);
	local_addr.sin_addr.s_addr = 0;
	bind(s, (sockaddr*)&local_addr, sizeof(local_addr));
	int c = listen(s, 5);
	std::cout << "Server receive ready" << std::endl;
	std::cout << std::endl;
	// извлекаем сообщение из очереди
	SOCKET client_socket; // сокет для клиента
	sockaddr_in client_addr; // адрес клиента(заполняется системой)
	int client_addr_size = sizeof(client_addr);
	// цикл извлечения запросов на подключение из очереди
	while ((client_socket = accept(s, (sockaddr*)&client_addr, &client_addr_size))) {
		numcl++;
		
		print();
		// Вызов нового потока для обслуживания клиента
		CreateThread(NULL, NULL, ThreadFunc, &client_socket, NULL, NULL);
	}
}