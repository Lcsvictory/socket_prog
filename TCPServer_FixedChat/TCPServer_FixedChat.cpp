#include "..\Common.h"

#define SERVERPORT 9000
#define BUFSIZE    50
#define SPLITER    ':'


char* recvId(char* buf) {  
	char ptr[128];
	int mLen;
	for (int i = 0; buf[i] != '\0'; ++i) {  
		if (buf[i] == SPLITER) {  
			strncpy(ptr, buf, (size_t)i);  
			ptr[i] = '\0'; // Null-terminate the string  
			mLen = strlen(&buf[i] + 1);
			memmove(buf, &buf[i + 1], mLen);
			buf[mLen] = '\0';
			return ptr;  
		}  
	}  
	return nullptr; // Return nullptr if SPLITER is not found  
}


int main(int argc, char* argv[]) {
	int retval;
	const char* serverId = "testServerId";
	if (argc > 1) {
		 serverId = argv[1];
	}
	
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}
	SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_socket == INVALID_SOCKET) err_quit("socket()");

	// bind()
	struct sockaddr_in serverAddr; //sockaddr_in ����ü�� IPv4�� ���� ����ü. sockaddr�� �ֻ�������ü.
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	//hton = ȣ��Ʈ -> ��Ʈ��ũ ����Ʈ �����Լ�.
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); //��� IP�ּҵ� �� �ްڴ� �̷�������.
	serverAddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_socket, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	//������ ��ſ� ����� ����
	SOCKET             client_sock;
	struct sockaddr_in clientAddr;
	int                addrlen;
	char               buf[BUFSIZE + 1];
	char			   *clientId;
	int                len;
	char               tempBuf[BUFSIZE + 1];

	while (true) {
		addrlen = sizeof(clientAddr);
		client_sock = accept(listen_socket, (struct sockaddr*)&clientAddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		//������ Ŭ���̾�Ʈ ���� ���
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientAddr.sin_addr, addr, sizeof(addr));
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", addr, ntohs(clientAddr.sin_port));

		//Ŭ���̾�Ʈ�� ������ ��� ����
		while (true) {
			retval = recv(client_sock, buf, BUFSIZE, MSG_WAITALL);
			clientId = recvId(buf);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				break;
			}
			else if (buf[0] == '\0') {
				break;
			}

			//���� ������ ���
			printf("%s : %s\n", clientId, buf);


			//���� ������ �Է¹ޱ�
			printf("%s : ", serverId);
			if (fgets(buf, BUFSIZE, stdin) == NULL) {
				break;
			}

			//'\n'���� ����
			len = (int)strlen(buf);
			if (buf[len - 1] == '\n') {
				buf[len - 1] = '\0';
			}
			

			strncpy(tempBuf, serverId, strlen(serverId));
			tempBuf[strlen(serverId)] = '\0';
			strncat(tempBuf, ":", (size_t)1);
			strncat(tempBuf, buf, strlen(buf));
			tempBuf[strlen(tempBuf)] = '\0';

			retval = send(client_sock, tempBuf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
				break;
			}
		}
		//���� �ݱ�
		closesocket(client_sock);
		printf("[TCP ����] Ŭ���̾�Ʈ ���� : IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", addr, ntohs(clientAddr.sin_port));

	}

	closesocket(listen_socket);


	//winsock ����
	WSACleanup();
	return 0;
}