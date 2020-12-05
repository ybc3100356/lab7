#ifndef LAB7_CLIENT_H
#define LAB7_CLIENT_H

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <vector>
#include <exception>

#include "../protocol/packet.hpp"

class Client
{
    bool connected;
    SOCKET servSock;
    std::vector<int> clientList;
public:
    Client();
    ~Client();
    int start();
    static Client* instance;
private:
    char parse(const std::string inputStr);
    void printMsg(const DataPacket& msg);
    void connectToServer();
    void refreshClientList(DataPacket& msg);
    friend DWORD WINAPI respondReceiver(LPVOID lpParameter);
};

class InstanceExist :public std::exception {};

typedef struct _respondData
{
    SOCKET servSock;
    _respondData(SOCKET _servSock) : servSock(_servSock) {}
    ~_respondData() {}
} RespondData, * PtrToRespondData;

DWORD WINAPI respondReceiver(LPVOID lpParameter);

#endif