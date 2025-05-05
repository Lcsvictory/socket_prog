#include "..\Common.h"

#define SERVERPORT 9000
#define BUFSIZE    512
#define SPLITER ':'

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
	char c, * ptr = buf;

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
	struct sockaddr_in serverAddr; //sockaddr_in 구조체는 IPv4를 위한 구조체. sockaddr은 최상위구조체.
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	//hton = 호스트 -> 네트워크 바이트 정렬함수.
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); //어느 IP주소든 다 받겠다 이런말같음.
	serverAddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_socket, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	//데이터 통신에 사용할 변수
	SOCKET             client_sock;
	struct sockaddr_in clientAddr;
	int                addrlen;
	int                len;
	char               buf[BUFSIZE + 1];
	char               tempBuf[BUFSIZE + 1];
	char*              clientId;

	while (true) {
		addrlen = sizeof(clientAddr);
		client_sock = accept(listen_socket, (struct sockaddr*)&clientAddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		//접속한 클라이언트 정보 출력
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientAddr.sin_addr, addr, sizeof(addr));
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", addr, ntohs(clientAddr.sin_port));

		//클라이언트와 데이터 통신 시작
		while (true) {
			//데이터 받기
			retval = recvline(client_sock, buf, BUFSIZE + 1);
			clientId = recvId(buf);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				break;
			}
			else if (retval == 1) { //줄바꿈 문자를 제거하지 않았기에 \n이 포함되어 온다.
				break;
			}

			//받은 데이터 출력
			printf("%s : %s", clientId, buf);


			//보낼 데이터 입력받기
			printf("%s : ", serverId);
			if (fgets(buf, BUFSIZE, stdin) == NULL) {
				break;
			}

			//'\n'문자 제거
			/*len = (int)strlen(buf);
			if (buf[len - 1] == '\n') {
				buf[len - 1] = '\0';
			}*/


			strncpy(tempBuf, serverId, strlen(serverId));
			tempBuf[strlen(serverId)] = '\0';
			strncat(tempBuf, ":", (size_t)1);
			strncat(tempBuf, buf, strlen(buf));
			tempBuf[strlen(tempBuf)] = '\0';

			//데이터 보내기
			retval = send(client_sock, tempBuf, (int)strlen(tempBuf), 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
				break;
			}
		}
		//소켓 닫기
		closesocket(client_sock);
		printf("[TCP 서버] 클라이언트 종료 : IP 주소=%s, 포트 번호=%d\n", addr, ntohs(clientAddr.sin_port));

	}

	closesocket(listen_socket);


	//winsock 종료
	WSACleanup();
	return 0;
}