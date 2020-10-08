#include "server.h"
#include "client.h"
#pragma comment(lib, "ws2_32.lib") //加载 ws2_32.dll
#pragma warning(disable:4996)

int main()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    Server *server = new Server();
    int ret = server->serve();

    WSACleanup();
    return ret;
}