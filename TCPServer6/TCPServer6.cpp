#include "..\Common.h"

#define SERVERPORT 9000
#define BUFSIZE    256

int main(int argc, char* argv[]) {
	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}
	SOCKET listen_socket = socket(AF_INET6, SOCK_STREAM, 0);
	if (listen_socket == INVALID_SOCKET) err_quit("socket()");

	// bind()
	struct sockaddr_in6 serverAddr; //sockaddr_in ����ü�� IPv4�� ���� ����ü. sockaddr�� �ֻ�������ü.
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin6_family = AF_INET6;
	//hton = ȣ��Ʈ -> ��Ʈ��ũ ����Ʈ �����Լ�.
	serverAddr.sin6_addr = in6addr_any; //��� IP�ּҵ� �� �ްڴ� �̷�������.
	serverAddr.sin6_port = htons(SERVERPORT);
	retval = bind(listen_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_socket, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	//������ ��ſ� ����� ����
	SOCKET             client_sock;
	struct sockaddr_in6 clientAddr;
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
		char addr[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET6, &clientAddr.sin6_addr, addr, sizeof(addr));
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", addr, ntohs(clientAddr.sin6_port));

		//Ŭ���̾�Ʈ�� ������ ��� ����
		while (true) {
			retval = recv(client_sock, buf, BUFSIZE, 0); //flags �ɼ�. ���� �ܰ迡�� �ٷ�� �ƴ�.
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				break;
			}
			else if (retval == 0) {
				break;
			}

			//���� ������ ���
			buf[retval] = '\0'; // retunvalue�� ���Ź��� ��Ʈ�� ������ ����Ʈ ������.
			printf("[TCP/%s:%d] %s\n", addr, ntohs(clientAddr.sin6_port), buf);

			//������ �ǵ����ֱ�
			retval = send(client_sock, buf, retval, 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
				break;
			}
		}
		//���� �ݱ�
		closesocket(client_sock);
		printf("[TCP ����] Ŭ���̾�Ʈ ���� : IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", addr, ntohs(clientAddr.sin6_port));

	}

	closesocket(listen_socket);


	//winsock ����
	WSACleanup();
	return 0;
}