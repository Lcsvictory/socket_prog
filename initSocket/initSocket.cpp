#include "..\Common.h"

SOCKET makeUDPSocket() {

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET) err_quit("socket()");
	printf("[알림] UDP소켓 생성 성공\n");

	return sock;
}

SOCKET makeTCPSocket() {

	//소켓을 만들자.
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) err_quit("socket()");
	printf("[알림] TCP소켓 생성 성공\n");
	return sock;

}

SOCKET betterThanMakeTCPSocket() {

	SOCKET sock = WSASocketA(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");
	printf("[알림] TCP소켓 생성 성공\n");
	return sock;

}

int f(int x) {
	if (x >= 0) return 0;
	WSASetLastError(WSAEINVAL);
	return SOCKET_ERROR;
}

int main(int argc, char* argv[]) {

	//에러발생코드
	/*int retval = f(-100);
	if (retval == SOCKET_ERROR) err_quit("f()");*/


	WSADATA wsa;
	//WORD 타입은 unsigend short. 정수형.
	//winsock 초기화. 
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return -1;
	}
	printf("[알림] winsock 초기화 성공\n\n");


	printf("### WSADATA가 가지고 있는 값 ###\n");
	printf("wsa.wVersion : %d\n", wsa.wVersion);
	printf("wsa.wHighVersion : %d\n", wsa.wHighVersion);
	printf("wsa.szDescription : %s\n", wsa.szDescription);
	printf("wsa.szSystemStatus : %s\n", wsa.szSystemStatus);



	SOCKET sock = betterThanMakeTCPSocket();
	if (sock == NULL) return -1;
	//소켓 닫기
	closesocket(sock);

	//winsock 종료
	WSACleanup();
	return 0;
}
