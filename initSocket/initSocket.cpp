#include "..\Common.h"

SOCKET makeUDPSocket() {

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET) err_quit("socket()");
	printf("[�˸�] UDP���� ���� ����\n");

	return sock;
}

SOCKET makeTCPSocket() {

	//������ ������.
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) err_quit("socket()");
	printf("[�˸�] TCP���� ���� ����\n");
	return sock;

}

SOCKET betterThanMakeTCPSocket() {

	SOCKET sock = WSASocketA(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");
	printf("[�˸�] TCP���� ���� ����\n");
	return sock;

}

int f(int x) {
	if (x >= 0) return 0;
	WSASetLastError(WSAEINVAL);
	return SOCKET_ERROR;
}

int main(int argc, char* argv[]) {

	//�����߻��ڵ�
	/*int retval = f(-100);
	if (retval == SOCKET_ERROR) err_quit("f()");*/


	WSADATA wsa;
	//WORD Ÿ���� unsigend short. ������.
	//winsock �ʱ�ȭ. 
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return -1;
	}
	printf("[�˸�] winsock �ʱ�ȭ ����\n\n");


	printf("### WSADATA�� ������ �ִ� �� ###\n");
	printf("wsa.wVersion : %d\n", wsa.wVersion);
	printf("wsa.wHighVersion : %d\n", wsa.wHighVersion);
	printf("wsa.szDescription : %s\n", wsa.szDescription);
	printf("wsa.szSystemStatus : %s\n", wsa.szSystemStatus);



	SOCKET sock = betterThanMakeTCPSocket();
	if (sock == NULL) return -1;
	//���� �ݱ�
	closesocket(sock);

	//winsock ����
	WSACleanup();
	return 0;
}
