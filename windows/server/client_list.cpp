#include "client_list.h"
#pragma warning(disable : 4996)
const size_t BUFFERSIZE = 50;

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

int ClientList::sendMessage(size_t index, const char* message, size_t len)
{
    int sent = 0, offset = 0;
    auto dataleft = htonl(len)+ sizeof(int);
    char msgSize[sizeof(int)];
    memcpy(msgSize, (char*)dataleft, sizeof(int));

    
    sent = send(clientPool[index].getSock(), msgSize, sizeof(int), NULL);
    while (dataleft > 0)
    {
        sent = send(clientPool[index].getSock(), message + offset, dataleft, NULL);
        offset += sent;
        dataleft -= sent;
    }
    return offset;
}

std::string ClientList::recvRequest(size_t index)
{
    int offset = 0;
    int recved;
    char msgSize[sizeof(int)];
    char buffer[BUFFERSIZE];
    std::string msg;

    recved = recv(clientPool[index].getSock(), msgSize, sizeof(int), NULL);
    int dataleft = *(int*)(msgSize);

    while (dataleft > 0)
    {
        //接收数据
        recved = recv(clientPool[index].getSock(), buffer + offset, dataleft, NULL);
        //msg += std::string(buffer + offset);
        if (recv == 0)
        {
            break;
        }
        offset += recved;
        dataleft -= recved;
    }

    msg = buffer;
    return msg;
}
