#include "..\Common.h"
#define TESTNAME "www.google.com"





bool IsLittleEndian() {
	int arr = 123456789;
	unsigned char* pByte = (unsigned char*) & arr;
	char outMem[1024] = { 0 };
	char inMem[1024] = { 0 };
	sprintf(outMem, "%X", arr);
	//123456789 숫자 리터럴 그 자체를 16진수로 변환한것.
	sprintf(inMem, "%X", *pByte);
	//숫자 리터럴을 저장할때 16진수로 메모리에 리틀엔디안 방식으로 저장됨.
	if (inMem[0] != outMem[0]) return true;
	

	return false;

}




// 도메인 이름 -> IPv4주소
bool GetIPAddr(const char* name, struct in_addr* addr) {
	struct hostent* ptr = gethostbyname(name);
	if (ptr == NULL) {
		err_display("gethostbyname()");
		return false;
	}
	if (ptr->h_addrtype != AF_INET) {
		return false;
	}
	memcpy(addr, ptr->h_addr, ptr->h_length);
	return true;
}

//IPv4 주소 -> 도메인 이름
bool GetDomainName(struct in_addr addr, char* name, int namelen) {
	struct hostent* ptr = gethostbyaddr((const char*)&addr, sizeof(addr), AF_INET);
	if (ptr == NULL) {
		err_display("gethostbyaddr()");
		return false;
	}
	if (ptr->h_addrtype != AF_INET) {
		return false;
	}
	strncpy(name, ptr->h_name, namelen);
	return true;
}


int main(int argc, char* argv[]) {

	if (IsLittleEndian()) {
		printf("LittleEndian 방식입니다.\n");
	}
	
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}
	printf("도메인 이름(변환 전) = %s\n", TESTNAME);

	//도메인 이름 -> IP 주소
	struct in_addr addr;
	if (GetIPAddr(TESTNAME, &addr)) {
		printf("raw IP 주소 : %u\n\n", addr.s_addr);

		//가져온 IP주소를 출력하기 위해 숫자를 문자열로 바꿔서 출력. 
		char str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &addr, str, sizeof(str));
		printf("IP 주소(변환 후) = %s\n", str);
		
		//가져온 IP주소를 다시 원래 도메인 주소로 출력.
		char name[256];
		if (GetDomainName(addr, name, sizeof(name))) {
			printf("도메인 이름(도메인 -> IP -> 도메인) = %s\n", name);
		}
			
	}
		
	WSACleanup();
	return 0;
}