#include "..\Common.h"

const char* SERVERIP = "127.0.0.1";
#define SERVERPORT  9000
#define BUFSIZE    50


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
	char buf[BUFSIZE];
	const char* testdata[] = {
		"데이터를 보내볼게요. 잘 받았는지 대답해주세요.",
		"잘 받았습니다.",
		"통신을 그만합시다.",
		"알겠어요. 잘가요.",
	};

	//서버와 데이터 통신
	for(int i = 0; i < sizeof(testdata) / sizeof(testdata[0]); i++) {
		memset(buf, '#', sizeof(buf));
		strncpy(buf, testdata[i], strlen(testdata[i]));

		//데이터 보내기
		retval = send(sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}

		printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);

	}
	closesocket(sock);

	WSACleanup();
	return 0;
}
