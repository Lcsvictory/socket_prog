#include <windows.h>
#include <stdio.h>

DWORD WINAPI MyThread(LPVOID pParam) {
	while (1);
	return 0;
}

int main(int argc, char* argv[]) {
	printf("우선순위 : %d~%d\n", THREAD_PRIORITY_IDLE, THREAD_PRIORITY_TIME_CRITICAL);

	SYSTEM_INFO si;
	GetSystemInfo(&si);

	for (int i = 0; i < (int)si.dwNumberOfProcessors; i++) {
		//스레드 생성
		HANDLE hThread1 = CreateThread(NULL, 0, MyThread, NULL, 0, NULL);

		//우선순위 높게 설정
		SetThreadPriority(hThread1, THREAD_PRIORITY_ABOVE_NORMAL);
		CloseHandle(hThread1);
	}

	//우선순위 낮게 설정
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
	Sleep(1000);
	printf("메인 스레드 실행!\n");
	return 0;
}