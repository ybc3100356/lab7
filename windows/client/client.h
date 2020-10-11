#ifndef LAB7_CLIENT_H
#define LAB7_CLIENT_H

#include <winsock2.h>
#include <iostream>
#include <string>

class Client
{
    SOCKET servSock;
public:
    Client() {}
    ~Client() {}
    int start();
};

#endif