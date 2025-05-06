#include <windows.h>
#include <stdio.h>

#define BUFSIZE 10

HANDLE hWriteEvent;
HANDLE hReadEvent;
int buf[BUFSIZE];


DWORD WINAPI WriteThread(LPVOID pParam) {
	DWORD retval;
	for (int k = 1; k <= 500; k++) {
		//�б� �Ϸ� ����Ѵ�.
		retval = WaitForSingleObject(hReadEvent, INFINITE);
		if (retval != WAIT_OBJECT_0) break;

		//���� ���ۿ� ������ ����
		for (int i = 0; i < BUFSIZE; i++) {
			buf[i] = k;
		}

		//���� �Ϸ� �˸�.
		SetEvent(hWriteEvent);
	}
	return 0;
}

DWORD WINAPI ReadThread(LPVOID pParam) {
	DWORD retval;
	while (true) {
		//���� �Ϸ� ���
		retval = WaitForSingleObject(hWriteEvent, INFINITE);
		if (retval != WAIT_OBJECT_0) break;

		//���� ������ ��� �� ���� 0���� �ʱ�ȭ��.
		printf("Thread %d:\t", GetCurrentThreadId());
		for (int i = 0; i < BUFSIZE; i++) {
			printf("%3d ", buf[i]);
		}
		printf("\n");
		memset(buf, 0, sizeof(buf));


		//�б� �Ϸ� �̺�Ʈ
		SetEvent(hReadEvent);
	}
	return 0;
}

int main(int argc, char** argv) {
	//�̺�Ʈ ����
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	hReadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	//������ ���� ����
	HANDLE hThread[3];
	hThread[0] = CreateThread(NULL, 0, WriteThread, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);
	hThread[2] = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);

	SetEvent(hReadEvent);

	//������ �� �� ���� ���
	WaitForMultipleObjects(3, hThread, TRUE, INFINITE);

	//�̺�Ʈ ����
	CloseHandle(hWriteEvent);
	CloseHandle(hReadEvent);
	return 0;
}
