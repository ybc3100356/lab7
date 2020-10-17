#ifndef LAB7_CLIENT_H
#define LAB7_CLIENT_H

#include <winsock2.h>
#include <iostream>
#include <string>
#include <exception>

#include "../protocol/packet.hpp"

class Client
{
    SOCKET servSock;
public:
    Client();
    ~Client();
    int start();
    char parse(std::string inputStr);
    void printMsg(DataPacket* msg);
    static Client* instance;
};

class InstanceExist :public std::exception {};

#endif