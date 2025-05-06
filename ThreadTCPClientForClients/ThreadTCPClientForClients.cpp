#include "..\Common.h"
#include <windows.h>
#include <stdio.h>

const char* SERVERIP = "127.0.0.1";
const char* SERVERDOMAIN = "lcsvictory.iptime.org";
#define SERVERPORT  9000
#define BUFSIZE    512
#define SPLITER ':'



char clientId[128];

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

bool GetIpAddr(const char* name, struct in_addr* addr) {
	struct hostent* ptr = gethostbyname(name);
	if (ptr == NULL) {
		err_display("gethostnyname()");
		return false;
	}
	if (ptr->h_addrtype != AF_INET) {
		return false;
	}
	memcpy(addr, ptr->h_addr, ptr->h_length);
	return true;
}

DWORD WINAPI InputSend(LPVOID pParam) {
	SOCKET sock = (SOCKET)pParam;
	char buf[BUFSIZE + 1];
	char tempBuf[BUFSIZE + 1];
	int retval;
	int  len;
	while (true) {
		//데이터 입력
		printf("\n");
		if (fgets(buf, BUFSIZE, stdin) == NULL) {
			break;
		}
		

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

	}
	return 0;
	
}

DWORD WINAPI RecvDisplay(LPVOID pParam) {
	SOCKET sock = (SOCKET)pParam;
	char buf[BUFSIZE + 1];
	int retval;
	int  len;
	char* otherId;

	while (true) {
		//데이터 받기
		retval = recvline(sock, buf, BUFSIZE + 1);
		otherId = recvId(buf);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0) {
			break;
		}

		//받은 데이터 출력
		buf[retval] = '\0';
		printf("%s : %s\n", otherId, buf);
	}
	return 0;
}
	


int main(int argc, char* argv[]) {
	int retval;
	strncpy(clientId, "test1", strlen("test1"));

	//명령행 인수가 있다면 사용한다.
	if (argc > 1) {
		memset(clientId, 0, sizeof(clientId));
		strncpy(clientId, argv[1], strlen(argv[1]));
	}
	else{
		printf("exit을 입력하면 종료합니다.\n");
		printf("ID를 입력하세요 : ");
		if (fgets(clientId, sizeof(clientId), stdin) == NULL) {
			return 1;
		}
		size_t len = strlen(clientId);
		if (len > 0 && clientId[len - 1] == '\n') {
			clientId[len - 1] = '\0';  // 줄바꿈 제거
		}
	}
	
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}

	//socket 생성
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	
	//connect()
	struct in_addr addr;
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr)); //초기화
	if (GetIpAddr(SERVERDOMAIN, &addr)) {//IP값 지정
		serveraddr.sin_addr = addr;
	}
	else {
		inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
	}
	serveraddr.sin_family = AF_INET; //IPv4방식 사용
	serveraddr.sin_port = htons(SERVERPORT); //포트번호
	retval = connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	//데이터 통신에 사용할 변수
	

	//서버와 데이터 통신
	
	HANDLE hThread[2];
	hThread[0] = CreateThread(NULL, 0, InputSend, (LPVOID)sock, 0, NULL);
	
	hThread[1] = CreateThread(NULL, 0, RecvDisplay, (LPVOID)sock, 0, NULL);


	WaitForMultipleObjects(2, hThread, FALSE, INFINITE);
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);

	closesocket(sock);

	WSACleanup();
	return 0;
}
