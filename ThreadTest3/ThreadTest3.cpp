#include <windows.h>
#include <stdio.h>

int sum = 0;

DWORD WINAPI MyThread(LPVOID pParam) {
	int num = (int)(long long)pParam;
	for (int i = 0; i <= num; i++) {
		sum += i;
	}
	return 0;
}

int main() {
	int num = 100;
	HANDLE hThread = CreateThread(NULL, 0, MyThread, (LPVOID)(long long)num, CREATE_SUSPENDED, NULL);

	printf("������ ���� �� ��� ��� = %d\n", sum);
	ResumeThread(hThread);
	WaitForSingleObject(hThread, INFINITE);
	printf("������ ���� �� ��� ��� = %d\n", sum);
	CloseHandle(hThread);
	return 0;
}