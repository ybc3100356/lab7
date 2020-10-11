#pragma comment(lib, "ws2_32.lib") //加载 ws2_32.dll
#include "server.h"
#include "client_list.h"

int main()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    Server *server = new Server();
    int ret = server->serve();
    
    delete server;
    WSACleanup();
    return ret;
}