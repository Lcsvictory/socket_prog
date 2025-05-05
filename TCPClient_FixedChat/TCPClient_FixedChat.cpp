#include "..\Common.h"

const char* SERVERIP = "127.0.0.1";
#define SERVERPORT  9000
#define BUFSIZE    50
#define SPLITER ':'




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
	const char* clientId = "clientTestId";
	if (argc > 1) {
		clientId = argv[1];
	}
	//����� �μ��� �ִٸ� ����Ѵ�.
	/*if (argc > 1) SERVERIP = argv[1];*/
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}

	//socket ����
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	//connect()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	//������ ��ſ� ����� ����
	char buf[BUFSIZE + 1];
	char tempBuf[BUFSIZE + 1];
	int  len;
	char* serverId;

	//������ ������ ���
	while (true) {
		//������ �Է�
		printf("%s : ", clientId);
		if (fgets(buf, BUFSIZE, stdin) == NULL) {
			break;
		}

		//'\n'���� ����
		len = (int)strlen(buf);
		if (buf[len - 1] == '\n') {
			buf[len - 1] = '\0';
		}
		
		strncpy(tempBuf, clientId, strlen(clientId));
		tempBuf[strlen(clientId)] = '\0';
		strncat(tempBuf, ":", (size_t)1);
		strncat(tempBuf, buf, strlen(buf));
		tempBuf[strlen(tempBuf)] = '\0';
		/*printf("%s\n", tempBuf);*/

		//������ ������
		retval = send(sock, tempBuf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}

		//printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);

		//������ �ޱ�
		retval = recv(sock, buf, BUFSIZE, MSG_WAITALL);
		serverId = recvId(buf);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0) {
			break;
		}

		//���� ������ ���
		
		
		printf("%s : %s\n", serverId, buf);

	}
	closesocket(sock);

	WSACleanup();
	return 0;
}
