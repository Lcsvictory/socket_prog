#include "..\Common.h"
#define TESTNAME "www.naver.com"

//도메인 이름으로 가져오는 함수
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

//IPv4주소로 가져오는 함수.
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
	if (ptr == NULL) { //IP주소를 입력했어.
		inet_pton(AF_INET, TESTNAME, &addr);
		ptr = GetDomainName(addr);
	}
	
	int i = 0;
	while (ptr->h_addr_list[i]) {
		struct in_addr ip_addr;
		memcpy(&ip_addr, ptr->h_addr_list[i], sizeof(struct in_addr));
		printf("%s의 address[%d]번째 : %s\n", TESTNAME, i, inet_ntoa(ip_addr));
		i++;
	}
	int j = 0;
	while (ptr->h_aliases[j]) {
		printf("%s의 aliases[%d]번째 : %s\n", TESTNAME, j, ptr->h_aliases[j]);
		j++;
	}
	/*printf("%s\n", ptr->h);*/
	

	WSACleanup();
	return 0;
}