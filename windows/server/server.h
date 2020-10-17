#ifndef LAB7_SERVER_H
#define LAB7_SERVER_H

#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <string>
#include <ctime>    

#include "../protocol/packet.hpp"

class Server {
    std::string name;
public:
    Server(std::string _name) :name(_name) {}
    ~Server() {}
    int serve();
    int respond(DataPacket *request, SOCKET clntSock, size_t index);
    std::string getName() { return name; }
};

typedef struct MyData
{
    SOCKET clntSock;
    size_t index;
    Server* server;
    MyData(SOCKET _clntSock, size_t _index, Server* _server) : clntSock(_clntSock), index(_index), server(_server) {}
    ~MyData() {}
} MYDATA, * PMYDATA;

DWORD WINAPI clientFunc(LPVOID lpParameter);

#endif