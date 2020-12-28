#ifndef LAB7_SERVER_H
#define LAB7_SERVER_H

#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <string>
#include <ctime>    
#include <chrono>

#include "../protocol/packet.hpp"

class Server {
    std::string name;
public:
    static Server* instance;

    Server(); 
    ~Server();
    int serve();
    int respond(DataPacket& request, SOCKET clntSock, size_t index);
    std::string getName() { return name; }
    friend DWORD WINAPI requestReceiver(LPVOID lpParameter);
};

typedef struct _respondData
{
    SOCKET clntSock;
    size_t index;
    Server* server;
    _respondData(SOCKET _clntSock, size_t _index, Server* _server) : clntSock(_clntSock), index(_index), server(_server) {}
    ~_respondData() {}
} RespondData, * PtrToRespondData;

DWORD WINAPI requestReceiver(LPVOID lpParameter);

#endif