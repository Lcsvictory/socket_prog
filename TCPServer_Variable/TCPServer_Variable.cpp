#include "..\Common.h"

#define SERVERPORT 9000
#define BUFSIZE    512

int _recv_ahead(SOCKET s, char* p) {
	__declspec(thread) static int nbytes = 0;
	__declspec(thread) static char buf[1024];
	__declspec(thread) static char* ptr;

	if (nbytes == 0 || nbytes == SOCKET_ERROR) {
		nbytes = recv(s, buf, sizeof(buf), 0);
		if (nbytes == SOCKET_ERROR) {
			return SOCKET_ERROR;
		}
		else if (nbytes == 0) {
			return 0;
		}
		ptr = buf;
	}
	--nbytes;
	*p = *ptr++;
	return 1;
}

int recvline(SOCKET s, char* buf, int maxlen) {
	int n, nbytes;
	char c, *ptr = buf;

	for (n = 1; n < maxlen; n++) {
		nbytes = _recv_ahead(s, &c);
		if (nbytes == 1) {
			*ptr++ = c;
			if (c == '\n') {
				break;
			}
		}
		else if (nbytes == 0) {
			*ptr = 0;
			return n - 1;
		}
		else {
			return SOCKET_ERROR;
		}
	}

	*ptr = 0;
	return n;
}


int main(int argc, char* argv[]) {
	int retval;

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
			retval = recvline(client_sock, buf, BUFSIZE + 1);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				break;
			}
			else if (retval == 0) {
				break;
			}

			//���� ������ ���
			printf("[TCP/%s:%d] %s\n", addr, ntohs(clientAddr.sin_port), buf);

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