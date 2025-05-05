#include "..\Common.h"

const char* SERVERIP = "127.0.0.1";
#define SERVERPORT  9000
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
	const char* clientId = "clientTestId";
	if (argc > 1) {
		clientId = argv[1];
	}
	//명령행 인수가 있다면 사용한다.
	if (argc > 1) SERVERIP = argv[1];
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}

	//socket 생성
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

	//데이터 통신에 사용할 변수
	char buf[BUFSIZE];
	char tempBuf[BUFSIZE];
	int  len;
	char* serverId;

	//서버와 데이터 통신
	while (true) {

		//보낼 데이터 입력받기
		printf("%s : ", clientId);
		if (fgets(buf, BUFSIZE, stdin) == NULL) {
			break;
		}

		//'\n'문자 제거
		/*len = (int)strlen(buf);
		if (buf[len - 1] == '\n') {
			buf[len - 1] = '\0';
		}*/


		strncpy(tempBuf, clientId, strlen(clientId));
		tempBuf[strlen(clientId)] = '\0';
		strncat(tempBuf, ":", (size_t)1);
		strncat(tempBuf, buf, strlen(buf));
		tempBuf[strlen(tempBuf)] = '\0';

		//데이터 보내기
		retval = send(sock, tempBuf, (int)strlen(tempBuf), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}

		//데이터 받기
		retval = recvline(sock, buf, BUFSIZE + 1);
		serverId = recvId(buf);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		

		//받은 데이터 출력
		printf("%s : %s", serverId, buf);


	}
	closesocket(sock);

	WSACleanup();
	return 0;
}
