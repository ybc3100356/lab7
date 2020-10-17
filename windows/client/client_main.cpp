#pragma comment(lib, "ws2_32.lib") //加载 ws2_32.dll
#include "client.h"

#include <stdlib.h>

void AtExitFunc()
{
    if (Client::instance != nullptr)
        Client::instance->~Client();
}

int main()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    atexit(AtExitFunc);

    Client client;
    int ret = client.start();

    WSACleanup();

    system("pause");
    return ret;
}