#ifndef LAB7_CLIENT_H_
#define LAB7_CLIENT_H_
#include "client.h"

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib") //╪сть ws2_32.dll
#pragma warning(disable:4996)

class Client
{
    Client() {}
    ~Client() {}
    int start();
};

#endif