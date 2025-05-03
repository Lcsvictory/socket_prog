#include "..\Common.h"

int main(int argc, char* argv[]) {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}
	//IPv4��ȯ ����
	
	//���� IPv4�ּ� ���
	const char* ipv4test = "137.43.21.32";
	//const char ipv4test[] = "137.43.21.32"; //�迭�� ����;
	printf("IPv4 �ּ�(��ȯ ��) = %s\n", ipv4test);

	struct in_addr ipv4num;
	// p to n. ���ڷ� ǥ���Ѵ�. 
	inet_pton(AF_INET, ipv4test, &ipv4num);
	printf("IPv4 �ּ�(��ȯ ��) = %#x\n", ipv4num.S_un.S_addr);
	
	// n to p. ���ڿ��� ǥ���Ѵ�.
	char ipv4str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &ipv4num, ipv4str, sizeof(ipv4str));
	printf("IPv4 �ּ�(�ٽ� ��ȯ ��) = %s\n", ipv4str);
	printf("\n");


	//IPv6��ȯ ����

	//���� IPv6�ּ� ���
	const char* ipv6test = "2001:0230:abcd:ffab:0023:eb00:ffff:1111";
	printf("IPv6 �ּ�(��ȯ ��) = %s\n", ipv6test);

	// p to n
	struct in6_addr ipv6num;
	inet_pton(AF_INET6, ipv6test, &ipv6num);
	printf("IPv6 �ּ�(��ȯ ��) = 0x");
	for (int i = 0; i < 16; i++) {
		printf("%02x", ipv6num.s6_addr[i]);
	}
	printf("\n");

	//n to p
	char ipv6str[INET6_ADDRSTRLEN];
	inet_ntop(AF_INET6, &ipv6num, ipv6str, sizeof(ipv6str));
	printf("IPv6 �ּ�(�ٽ� ��ȯ ��) = %s\n", ipv6str);



	WSACleanup();
	return 0;

}