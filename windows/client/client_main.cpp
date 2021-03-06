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
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0)
    {
        atexit(AtExitFunc);

        Client client;
        int ret = client.start();

        WSACleanup();
        system("pause");
        return ret;
    }
    else
    {
        std::cout << "WSAStartup wrong!" << std::endl;
        return 0;
    }
}