//init
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include "Client.h"
#include "Util.cpp"

#pragma comment(lib,"Ws2_32.lib")

//server
#define PORT "7727"

//recv send
#define BUFLEN 512

int ClientClass::StartClient() {

	//init
	WSADATA wsaData;
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup Failed: %d\n", iResult);
		return 1;
	}

	//client
	struct addrinfo* result = NULL;
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;


	iResult = getaddrinfo("127.0.0.1", PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	SOCKET ClientSocket = INVALID_SOCKET;
	ClientSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ClientSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	//connect
	iResult = connect(ClientSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("connect failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}
	freeaddrinfo(result);

	if (ClientSocket == SOCKET_ERROR) {
		printf("connect failed with error: %ld\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	//recv send

	char recvbuf[BUFLEN];
	int iSendResult;
	int recvbuflen = BUFLEN;
	const char* sendbuf = "this is a test";
	char inputStr[50];

	iResult = 1;
	while (iResult > 0) {

		scanf("%s", &inputStr);

		iSendResult = send(ClientSocket, inputStr, (int)strlen(inputStr), 0);
		if (iSendResult == SOCKET_ERROR) {
			printf("send failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}
		printf("Bytes send: %d\n", iSendResult);


		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);
		}
		else if (iResult == 0) {
			printf("Connection closing...\n");
		}
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}
		Util::TrimChars(recvbuf, (int)strlen(recvbuf));

	}

	//close
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	closesocket(ClientSocket);
	WSACleanup();
	return 0;
}