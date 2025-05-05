#include <Windows.h>
#include <stdio.h>
#include <process.h>

#if false
//기본적인 쓰레드 생성 방법
struct Pointer3D {
	int x, y, z;
};

DWORD WINAPI MyThread(LPVOID pParam) {
	Sleep(1000);
	Pointer3D* pt = (Pointer3D*)pParam;
	printf("Running MyThread() %d: %d, %d, %d\n", GetCurrentThreadId(), pt->x, pt->y, pt->z);
	return 0;
}

int main(int argc, char* argv[]) {
	Pointer3D pt1 = { 10, 20, 30 };
	HANDLE hThread1 = CreateThread(NULL, 0, MyThread, &pt1, 0, NULL);
	if (hThread1 == NULL) return 1;
	CloseHandle(hThread1);

	Pointer3D pt2 = { 40, 50, 60 };
	HANDLE hThread2 = CreateThread(NULL, 0, MyThread, &pt2, 0, NULL);
	if (hThread2 == NULL) return 1;
	CloseHandle(hThread2);

	printf("Running main() %d\n", GetCurrentThreadId());
	Sleep(2000);
	return 0;
}
#endif
//C++에서 사용하는 쓰레드 생성 방법
struct Pointer3D {
	int x, y, z;
};

unsigned __stdcall MyThread(void* pParam) {
	Sleep(1000);
	Pointer3D* pt = (Pointer3D*)pParam;
	printf("Running MyThread() %d: %d, %d, %d\n", GetCurrentThreadId(), pt->x, pt->y, pt->z);
	return 0;
}

int main(int argc, char* argv[]) {
	Pointer3D pt1 = { 10, 20, 30 };
	HANDLE hThread1 = (HANDLE)_beginthreadex(NULL, 0, MyThread, &pt1, 0, NULL);
	if (hThread1 == NULL) return 1;
	CloseHandle(hThread1);

	Pointer3D pt2 = { 40, 50, 60 };
	HANDLE hThread2 = (HANDLE)_beginthreadex(NULL, 0, MyThread, &pt2, 0, NULL);
	if (hThread2 == NULL) return 1;
	CloseHandle(hThread2);

	printf("Running main() %d\n", GetCurrentThreadId());
	Sleep(2000);
	return 0;
}