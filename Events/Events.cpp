#include <windows.h>
#include <stdio.h>

#define BUFSIZE 10

HANDLE hWriteEvent;
HANDLE hReadEvent;
int buf[BUFSIZE];


DWORD WINAPI WriteThread(LPVOID pParam) {
	DWORD retval;
	for (int k = 1; k <= 500; k++) {
		//읽기 완료 대기한다.
		retval = WaitForSingleObject(hReadEvent, INFINITE);
		if (retval != WAIT_OBJECT_0) break;

		//공유 버퍼에 데이터 쓰기
		for (int i = 0; i < BUFSIZE; i++) {
			buf[i] = k;
		}

		//쓰기 완료 알림.
		SetEvent(hWriteEvent);
	}
	return 0;
}

DWORD WINAPI ReadThread(LPVOID pParam) {
	DWORD retval;
	while (true) {
		//쓰기 완료 대기
		retval = WaitForSingleObject(hWriteEvent, INFINITE);
		if (retval != WAIT_OBJECT_0) break;

		//읽은 데이터 출력 후 버퍼 0으로 초기화함.
		printf("Thread %d:\t", GetCurrentThreadId());
		for (int i = 0; i < BUFSIZE; i++) {
			printf("%3d ", buf[i]);
		}
		printf("\n");
		memset(buf, 0, sizeof(buf));


		//읽기 완료 이벤트
		SetEvent(hReadEvent);
	}
	return 0;
}

int main(int argc, char** argv) {
	//이벤트 생성
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	hReadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	//스레드 세개 생성
	HANDLE hThread[3];
	hThread[0] = CreateThread(NULL, 0, WriteThread, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);
	hThread[2] = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);

	SetEvent(hReadEvent);

	//스레드 세 개 종료 대기
	WaitForMultipleObjects(3, hThread, TRUE, INFINITE);

	//이벤트 제거
	CloseHandle(hWriteEvent);
	CloseHandle(hReadEvent);
	return 0;
}
