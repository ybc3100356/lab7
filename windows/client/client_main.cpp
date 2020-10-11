#pragma comment(lib, "ws2_32.lib") //加载 ws2_32.dll
#include "client.h"

#include <stdlib.h>

int main()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    Client client;
    int ret = client.start();

    WSACleanup();

    system("pause");
    return ret;
}