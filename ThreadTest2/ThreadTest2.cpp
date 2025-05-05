#include <windows.h>
#include <stdio.h>

DWORD WINAPI MyThread(LPVOID pParam) {
	while (1);
	return 0;
}

int main(int argc, char* argv[]) {
	printf("�켱���� : %d~%d\n", THREAD_PRIORITY_IDLE, THREAD_PRIORITY_TIME_CRITICAL);

	SYSTEM_INFO si;
	GetSystemInfo(&si);

	for (int i = 0; i < (int)si.dwNumberOfProcessors; i++) {
		//������ ����
		HANDLE hThread1 = CreateThread(NULL, 0, MyThread, NULL, 0, NULL);

		//�켱���� ���� ����
		SetThreadPriority(hThread1, THREAD_PRIORITY_ABOVE_NORMAL);
		CloseHandle(hThread1);
	}

	//�켱���� ���� ����
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
	Sleep(1000);
	printf("���� ������ ����!\n");
	return 0;
}