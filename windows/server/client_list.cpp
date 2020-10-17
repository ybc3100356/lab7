#include "client_list.h"
#include "../protocol/my_protocol.hpp"
#pragma warning(disable : 4996)

ClientList::Client ClientList::clientPool[ClientList::MAXUSERNUM];

int ClientList::removeClient(size_t index)
{
    if (index < MAXUSERNUM)
    {
        clientPool[index].setConnected(false);
        return 0;
    }
    return -1;
}

int ClientList::addClient(SOCKET clntSock, SOCKADDR clntAddr)
{
    for (size_t i = 0; i < MAXUSERNUM; i++)
    {
        if (!clientPool[i].isConnected())
        {
            clientPool[i].setAddr(clntAddr);
            clientPool[i].setSock(clntSock);
            clientPool[i].setConnected(true);
            return (int)i;
        }
    }
    return -1;
}

std::string ClientList::getClientList()
{
    std::string clientListStr;
    for (size_t i = 0; i < MAXUSERNUM; i++)
    {
        if (clientPool[i].isConnected())
        {
            //±àºÅ¡¢IPµØÖ·¡¢¶Ë¿Ú
            sockaddr_in* addr_in = (sockaddr_in*)(&clientPool[i].getAddr());
            std::string number = std::to_string(i);
            std::string addr = inet_ntoa(addr_in->sin_addr);
            std::string port = std::to_string(addr_in->sin_port);

            clientListStr += "number: " + number + ", addr: " + addr + ", port:" + port + "\n";
        }
    }
    return clientListStr;
}
