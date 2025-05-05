#include "..\Common.h"

const char* SERVERIP = "127.0.0.1";
#define SERVERPORT  9000
#define BUFSIZE    50


int main(int argc, char* argv[]) {

	int retval;

	//����� �μ��� �ִٸ� ����Ѵ�.
	if (argc > 1) SERVERIP = argv[1];
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}

	//socket ����
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

	//������ ��ſ� ����� ����
	char buf[BUFSIZE];
	const char* testdata[] = {
		"�����͸� �������Կ�. �� �޾Ҵ��� ������ּ���.",
		"�� �޾ҽ��ϴ�.",
		"����� �׸��սô�.",
		"�˰ھ��. �߰���.",
	};

	//������ ������ ���
	for(int i = 0; i < sizeof(testdata) / sizeof(testdata[0]); i++) {
		memset(buf, '#', sizeof(buf));
		strncpy(buf, testdata[i], strlen(testdata[i]));

		//������ ������
		retval = send(sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}

		printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);

	}
	closesocket(sock);

	WSACleanup();
	return 0;
}
