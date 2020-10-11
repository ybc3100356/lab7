#ifndef LAB7_SERVER_H
#define LAB7_SERVER_H

#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>

class Server{
public:
    Server(){}
    ~Server(){}
    int serve();
};

typedef struct MyData
{
    SOCKET clntSock;
    size_t index;
    MyData(SOCKET _clntSock, size_t _index) : clntSock(_clntSock), index(_index) {}
    ~MyData() {}
} MYDATA, *PMYDATA;

DWORD WINAPI clientFunc(LPVOID lpParameter);

#endif