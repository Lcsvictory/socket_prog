#include "..\Common.h"

int main(int argc, char* argv[]) {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}
	u_short x1 = 0x1234;
	u_long  y1 = 0x12345678;
	u_short x2;
	u_long y2;

	//호스트 바이트 -> 네트워크 바이트 hton()
	printf("[호스트바이트 -> 네트워크 바이트]\n");
	printf("%#x -> %#x\n", x1, x2 = htons(x1));
	printf("%#x -> %#x\n", y1, y2 = htons(y1));
	

	//네트워크 바이트 -> 호스트 바이트
	printf("\n[네트워크 바이트 -> 호스트 바이트]\n");
	printf("%#x -> %#x\n", x1, x2 = ntohs(x2));
	printf("%#x -> %#x\n", y1, y2 = ntohl(y2));

	WSACleanup();
	return 0;
}
