#include "client.h"
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
