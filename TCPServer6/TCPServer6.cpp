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
	struct sockaddr_in6 serverAddr; //sockaddr_in 구조체는 IPv4를 위한 구조체. sockaddr은 최상위구조체.
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin6_family = AF_INET6;
	//hton = 호스트 -> 네트워크 바이트 정렬함수.
	serverAddr.sin6_addr = in6addr_any; //어느 IP주소든 다 받겠다 이런말같음.
	serverAddr.sin6_port = htons(SERVERPORT);
	retval = bind(listen_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_socket, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	//데이터 통신에 사용할 변수
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

		//접속한 클라이언트 정보 출력
		char addr[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET6, &clientAddr.sin6_addr, addr, sizeof(addr));
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", addr, ntohs(clientAddr.sin6_port));

		//클라이언트와 데이터 통신 시작
		while (true) {
			retval = recv(client_sock, buf, BUFSIZE, 0); //flags 옵션. 지금 단계에선 다룰게 아님.
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				break;
			}
			else if (retval == 0) {
				break;
			}

			//받은 데이터 출력
			buf[retval] = '\0'; // retunvalue는 수신받은 스트림 데이터 바이트 사이즈.
			printf("[TCP/%s:%d] %s\n", addr, ntohs(clientAddr.sin6_port), buf);

			//데이터 되돌려주기
			retval = send(client_sock, buf, retval, 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
				break;
			}
		}
		//소켓 닫기
		closesocket(client_sock);
		printf("[TCP 서버] 클라이언트 종료 : IP 주소=%s, 포트 번호=%d\n", addr, ntohs(clientAddr.sin6_port));

	}

	closesocket(listen_socket);


	//winsock 종료
	WSACleanup();
	return 0;
}