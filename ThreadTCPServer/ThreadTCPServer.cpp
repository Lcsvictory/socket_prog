#include "..\Common.h"

#define SERVERPORT 9000
#define BUFSIZE    512

//데이터 통신
DWORD WINAPI ProcessClient(LPVOID pParam) {
	SOCKET client_sock = (SOCKET)pParam;
	sockaddr_in clientaddr;
	memset(&clientaddr, 0, sizeof(clientaddr));
	int    retval;
	char   addr[INET_ADDRSTRLEN];
	int    addrlen;
	char   buf[BUFSIZE + 1];
	
	//클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

	while (true) {
		//데이터 받기
		retval = recv(client_sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0) {
			break;
		}
		
		//받은 데이터 출력
		buf[retval] = '\0';
		printf("[TCP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), buf);

		//데이터 보내기
		retval = send(client_sock, buf, retval, 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}
	}
	
	//소켓 닫기
	closesocket(client_sock);
	printf("[TCP 서버] 클라이언트 종료 : IP주소=%s, 포트 번호=%d\n", addr, ntohs(clientaddr.sin_port));
	return 0;
}

int main(int argc, char* argv[]) {
	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}

	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	//bind()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	//listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	//데이터 통신에 사용할 변수들
	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;
	HANDLE hThread;

	while (true) {
		//accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		//접속한 클라이언트 정보 출력
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("[TCP 서버] 클라이언트 접속 : IP주소=%s, 포트 번호=%d\n", addr, ntohs(clientaddr.sin_port));


		//스레드 생성
		hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
		if (hThread == NULL) {
			closesocket(client_sock);
		}
		else {
			CloseHandle(hThread);
		}
	}
	
	//소켓 닫기
	closesocket(listen_sock);

	WSACleanup();
	return 0;

}