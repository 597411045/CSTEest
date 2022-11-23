//init
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define _WINSOCK_DEPRECATED_NO_WARNINGS


#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include "Server.h"

#pragma comment(lib,"Ws2_32.lib")

//server
#define PORT "7727"
#define IP "127.0.0.1"

//recv send
#define BUFLEN 512

int ServerClass::StartServer() {

	//init
	WSADATA wsaData;
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup Failed: %d\n", iResult);
		return 1;
	}

	//server
	struct addrinfo* result = NULL;
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	iResult = getaddrinfo(NULL, PORT, &hints, &result);
	sockaddr_in* tmpAddr_in = (struct sockaddr_in*)result->ai_addr;
	printf("%s", inet_ntoa(tmpAddr_in->sin_addr));
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	//bind
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	freeaddrinfo(result);

	//listen
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	//accept
	SOCKET AcceptedSocket;
	AcceptedSocket = INVALID_SOCKET;
	AcceptedSocket = accept(ListenSocket, NULL, NULL);
	if (AcceptedSocket == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	closesocket(ListenSocket);

	//recv send

	char recvbuf[BUFLEN];
	const char* sendbuf = "Welcome to Socket";
	int iSendResult;
	int recvbuflen = BUFLEN;

	iResult = 1;
	while (iResult > 0) {
		iResult = recv(AcceptedSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);
			for (int i = 0; i < 20; i++) {
				printf("%i ", recvbuf[i]);
			}

			iSendResult = send(AcceptedSocket, sendbuf, (int)strlen(sendbuf), 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(AcceptedSocket);
				WSACleanup();
				return 1;
			}
			printf("Bytes send: %d\n", iSendResult);
		}
		else if (iResult == 0) {
			printf("Connection closing...\n");
		}
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(AcceptedSocket);
			WSACleanup();
			return 1;
		}
	}

	//close
	iResult = shutdown(AcceptedSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(AcceptedSocket);
		WSACleanup();
		return 1;
	}

	closesocket(AcceptedSocket);
	WSACleanup();
	return 0;
}