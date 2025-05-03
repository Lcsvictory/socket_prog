#include "..\Common.h"
#define TESTNAME "www.naver.com"

//������ �̸����� �������� �Լ�
struct hostent* GetIPAddr(const char* name) {
	struct hostent* ptr = gethostbyname(name);
	if (ptr == NULL) {
		err_display("gethostbyname()");
		return NULL;
	}
	if (ptr->h_addrtype != AF_INET) {
		return NULL;
	}
	return ptr;
}

//IPv4�ּҷ� �������� �Լ�.
struct hostent* GetDomainName(struct in_addr addr) {
	struct hostent* ptr = gethostbyaddr((const char*)&addr, sizeof(addr), AF_INET);
	if (ptr == NULL) {
		err_display("gethostbyaddr()");
		return NULL;
	}
	if (ptr->h_addrtype != AF_INET) {
		return NULL;
	}
	return ptr;
}


int main(int argc, char* argv[]) {


	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}
	struct in_addr addr;
	struct hostent* ptr = GetIPAddr(TESTNAME);
	if (ptr == NULL) { //IP�ּҸ� �Է��߾�.
		inet_pton(AF_INET, TESTNAME, &addr);
		ptr = GetDomainName(addr);
	}
	
	int i = 0;
	while (ptr->h_addr_list[i]) {
		struct in_addr ip_addr;
		memcpy(&ip_addr, ptr->h_addr_list[i], sizeof(struct in_addr));
		printf("%s�� address[%d]��° : %s\n", TESTNAME, i, inet_ntoa(ip_addr));
		i++;
	}
	int j = 0;
	while (ptr->h_aliases[j]) {
		printf("%s�� aliases[%d]��° : %s\n", TESTNAME, j, ptr->h_aliases[j]);
		j++;
	}
	/*printf("%s\n", ptr->h);*/
	

	WSACleanup();
	return 0;
}