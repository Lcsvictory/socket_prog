#include "..\Common.h"

#define SERVERPORT 9000
#define BUFSIZE    512
#define CLIENTCNT  10

// -- 구조체 정의를 전역변수 선언 전에 배치 --

typedef struct Node {
    void* data;
    Node* next;
} Node;

typedef struct ll {
    Node* head;
    Node* tail;
    int   size;
} ll;

// -- 전역 변수 --
ll client_socks = { NULL, NULL, 0 };
ll clientAddrs = { NULL, NULL, 0 };
int clientIndex = 0;

// append, remove, peek 함수는 변경 없이 그대로 사용
void append(ll* l, void* d) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (node == NULL) {
        return;
    }
    node->data = d;
    node->next = NULL;
    if (l->head == NULL) {
        l->head = node;
        l->tail = node;
    }
    else {
        l->tail->next = node;
        l->tail = node;
    }
    l->size++;
}
bool remove(ll* l, void* d) {
    if (l->head == NULL) return false;
    Node* current = l->head;
    if (l->head->data == d) {
        l->head = current->next;
        free(current->data);
        free(current);
        l->size--;
        return true;
    }
    Node* previous = NULL;
    while (current) {
        if (current->data == d) {
            previous->next = current->next;
            free(current->data);
            free(current);
            l->size--;
            return true;
        }
        previous = current;
        current = current->next;
    }
    return false;
}
void* peek(ll* l, int index) {
    if (index < 0 || index >= l->size) {
        printf("Index out of range.\n");
        return NULL;
    }
    Node* current = l->head;
    for (int i = 0; i < index; ++i) {
        current = current->next;
    }
    return current->data;
}

int _recv_ahead(SOCKET s, char* p) {
    __declspec(thread) static int nbytes = 0;
    __declspec(thread) static char buf[1024];
    __declspec(thread) static char* ptr;

    if (nbytes == 0 || nbytes == SOCKET_ERROR) {
        nbytes = recv(s, buf, sizeof(buf), 0);
        if (nbytes == SOCKET_ERROR) {
            return SOCKET_ERROR;
        }
        else if (nbytes == 0) {
            return 0;
        }
        ptr = buf;
    }
    --nbytes;
    *p = *ptr++;
    return 1;
}

int recvline(SOCKET s, char* buf, int maxlen) {
    int n, nbytes;
    char c, * ptr = buf;

    for (n = 1; n < maxlen; n++) {
        nbytes = _recv_ahead(s, &c);
        if (nbytes == 1) {
            *ptr++ = c;
            if (c == '\n') {
                break;
            }
        }
        else if (nbytes == 0) {
            *ptr = 0;
            return n - 1;
        }
        else {
            return SOCKET_ERROR;
        }
    }
    *ptr = 0;
    return n;
}

DWORD WINAPI ProcessClient(LPVOID pParam) {
    SOCKET self_client_sock = (SOCKET)pParam;
    sockaddr_in clientaddr;
    memset(&clientaddr, 0, sizeof(clientaddr));

    // -- 로컬 변수를 힙에 복사하여 저장 --
    SOCKET* psock = (SOCKET*)malloc(sizeof(SOCKET));
    *psock = self_client_sock;
    append(&client_socks, psock);

    sockaddr_in* paddr = (sockaddr_in*)malloc(sizeof(sockaddr_in));
    *paddr = clientaddr;
    append(&clientAddrs, paddr);

    int    retval;
    char   addr[INET_ADDRSTRLEN];
    char   buf[BUFSIZE + 1];
    int    selfClientIndex = clientIndex;

    // 클라이언트 정보 얻기
    int addrlen = sizeof(clientaddr);
    getpeername(self_client_sock, (struct sockaddr*)&clientaddr, &addrlen);
    inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

    while (true) {
        retval = recvline(self_client_sock, buf, BUFSIZE);
        if (retval == SOCKET_ERROR) {
            err_display("recv()");
            break;
        }
        else if (retval > 4 && strncmp(&buf[retval - 5], "exit", 4) == 0) {
            break;
        }
        for (int i = 0; i < client_socks.size; i++) {
            SOCKET temp = *(SOCKET*)peek(&client_socks, i);
            if (temp == INVALID_SOCKET) continue;
            if (temp != self_client_sock) {
                retval = send(temp, buf, retval, 0);
                if (retval == SOCKET_ERROR) {
                    err_display("send()");
                    break;
                }
            }
        }
    }

    closesocket(self_client_sock);
    remove(&client_socks, psock);
    printf("[TCP 서버] 클라이언트 종료 : IP주소=%s, 포트 번호=%d\n", addr, ntohs(clientaddr.sin_port));
    remove(&clientAddrs, paddr);
    return 0;
}

int main(int argc, char* argv[]) {
    WSAData wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        return 1;
    }
    SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket == INVALID_SOCKET) err_quit("socket()");

    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(SERVERPORT);
    if (bind(listen_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        err_quit("bind()");

    if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR)
        err_quit("listen()");

    char buf[BUFSIZE + 1];
    while (true) {
        // -- accept 전에 지역 변수에 client info 받기 --
        sockaddr_in clientaddr;
        int addrlen = sizeof(clientaddr);
        SOCKET newSock = accept(listen_socket, (struct sockaddr*)&clientaddr, &addrlen);
        if (newSock == INVALID_SOCKET) {
            err_display("accept()");
            break;
        }

        // 접속 정보 출력
        char addrStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientaddr.sin_addr, addrStr, sizeof(addrStr));
        printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
            addrStr, ntohs(clientaddr.sin_port));

        HANDLE hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)newSock, 0, NULL);
        if (hThread == NULL) {
            closesocket(newSock);
        }
        else {
            CloseHandle(hThread);
            clientIndex++;
        }
    }

    closesocket(listen_socket);
    WSACleanup();
    return 0;
}
