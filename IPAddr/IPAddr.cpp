#include "..\Common.h"

int main(int argc, char* argv[]) {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}
	//IPv4변환 연습
	
	//원래 IPv4주소 출력
	const char* ipv4test = "137.43.21.32";
	//const char ipv4test[] = "137.43.21.32"; //배열도 가능;
	printf("IPv4 주소(변환 전) = %s\n", ipv4test);

	struct in_addr ipv4num;
	// p to n. 숫자로 표시한다. 
	inet_pton(AF_INET, ipv4test, &ipv4num);
	printf("IPv4 주소(변환 후) = %#x\n", ipv4num.S_un.S_addr);
	
	// n to p. 문자열로 표시한다.
	char ipv4str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &ipv4num, ipv4str, sizeof(ipv4str));
	printf("IPv4 주소(다시 변환 후) = %s\n", ipv4str);
	printf("\n");


	//IPv6변환 연습

	//원래 IPv6주소 출력
	const char* ipv6test = "2001:0230:abcd:ffab:0023:eb00:ffff:1111";
	printf("IPv6 주소(변환 전) = %s\n", ipv6test);

	// p to n
	struct in6_addr ipv6num;
	inet_pton(AF_INET6, ipv6test, &ipv6num);
	printf("IPv6 주소(변환 후) = 0x");
	for (int i = 0; i < 16; i++) {
		printf("%02x", ipv6num.s6_addr[i]);
	}
	printf("\n");

	//n to p
	char ipv6str[INET6_ADDRSTRLEN];
	inet_ntop(AF_INET6, &ipv6num, ipv6str, sizeof(ipv6str));
	printf("IPv6 주소(다시 변환 후) = %s\n", ipv6str);



	WSACleanup();
	return 0;

}