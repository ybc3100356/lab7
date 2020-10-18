#pragma comment(lib, "ws2_32.lib") //加载 ws2_32.dll
#include "server.h"
#include "client_list.h"

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0)
    {
        Server server("S E R V E R");
        int ret = server.serve();
        WSACleanup();
        return ret;
    }
    else
    {
        std::cout << "WSAStartup wrong!" << std::endl;
        return 0;
    }
}