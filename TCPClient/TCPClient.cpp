#include "..\Common.h"

const char* SERVERIP = "127.0.0.1";
#define SERVERPORT  9000
#define BUFSIZE    512


int main(int argc, char* argv[]) {

	int retval;

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
	char buf[BUFSIZE + 1];
	int  len;

	//서버와 데이터 통신
	while (true) {
		//데이터 입력
		printf("\n[보낼 데이터]");
		if (fgets(buf, BUFSIZE, stdin) == NULL) {
			break;
		}
		
		//'\n'문자 제거
		len = (int)strlen(buf);
		if (buf[len - 1] == '\n') {
			buf[len - 1] = '\0';
		}
		if (strlen(buf) == 0) {
			break;
		}
		
		//데이터 보내기
		retval = send(sock, buf, (int)strlen(buf), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}
		
		printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);

		//데이터 받기
		retval = recv(sock, buf, retval, MSG_WAITALL);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0) {
			break;
		}
		
		//받은 데이터 출력
		buf[retval] = '\0';
		printf("[TCP 클라이언트] %d바이트를 받았습니다.\n", retval);
		printf("[받은 데이터] %s\n", buf);
	
	}
	closesocket(sock);

	WSACleanup();
	return 0;
}
