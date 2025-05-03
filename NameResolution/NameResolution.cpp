#include "..\Common.h"
#define TESTNAME "www.google.com"





bool IsLittleEndian() {
	int arr = 123456789;
	unsigned char* pByte = (unsigned char*) & arr;
	char outMem[1024] = { 0 };
	char inMem[1024] = { 0 };
	sprintf(outMem, "%X", arr);
	//123456789 ���� ���ͷ� �� ��ü�� 16������ ��ȯ�Ѱ�.
	sprintf(inMem, "%X", *pByte);
	//���� ���ͷ��� �����Ҷ� 16������ �޸𸮿� ��Ʋ����� ������� �����.
	if (inMem[0] != outMem[0]) return true;
	

	return false;

}




// ������ �̸� -> IPv4�ּ�
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

//IPv4 �ּ� -> ������ �̸�
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
		printf("LittleEndian ����Դϴ�.\n");
	}
	
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}
	printf("������ �̸�(��ȯ ��) = %s\n", TESTNAME);

	//������ �̸� -> IP �ּ�
	struct in_addr addr;
	if (GetIPAddr(TESTNAME, &addr)) {
		printf("raw IP �ּ� : %u\n\n", addr.s_addr);

		//������ IP�ּҸ� ����ϱ� ���� ���ڸ� ���ڿ��� �ٲ㼭 ���. 
		char str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &addr, str, sizeof(str));
		printf("IP �ּ�(��ȯ ��) = %s\n", str);
		
		//������ IP�ּҸ� �ٽ� ���� ������ �ּҷ� ���.
		char name[256];
		if (GetDomainName(addr, name, sizeof(name))) {
			printf("������ �̸�(������ -> IP -> ������) = %s\n", name);
		}
			
	}
		
	WSACleanup();
	return 0;
}