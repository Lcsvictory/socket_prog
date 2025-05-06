
#include <windows.h>
#include <stdio.h>

#define MAXCNT 100000000 //100,000,000
int count = 0;
CRITICAL_SECTION cs;

DWORD WINAPI MyThread1(LPVOID pParam) {
	for (int i = 0; i < MAXCNT; i++) {
		EnterCriticalSection(&cs); //내가 쓸거니까 아무도 쓰지마!
		count += 2;
		LeaveCriticalSection(&cs); //이제 다 썼어.
	}
	return 0;
}

DWORD WINAPI MyThread2(LPVOID pParam) {
	for (int i = 0; i < MAXCNT; i++) {
		EnterCriticalSection(&cs);
		count -= 2;
		LeaveCriticalSection(&cs);
	}
	return 0;
}

int main() {

	//임계 영역 초기화
	InitializeCriticalSection(&cs);
	
	//스레드 2개 생성
	HANDLE hThread[2];
	hThread[0] = CreateThread(NULL, 0, MyThread1, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, MyThread2, NULL, 0, NULL);

	//스레드 종료 대기
	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);

	//임계 영역 삭제
	DeleteCriticalSection(&cs);

	//결과 출력
	printf("count = %d\n", count);
	return 0;
}


