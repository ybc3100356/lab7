#include "server.h"
#include "client_list.h"
#pragma warning(disable : 4996)

int Server::serve()
{
    //创建套接字
    SOCKET servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    //绑定套接字
    sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = PF_INET;
    //sockAddr.sin_addr.S_un.S_addr = inet_addr("192.168.1.159");
    //sockAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    sockAddr.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");
    sockAddr.sin_port = htons(1730);
    bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

    //监听
    listen(servSock, ClientList::MAXUSERNUM);

    //接受客户端请求
    SOCKADDR clntAddr;
    int nSize = sizeof(SOCKADDR);
    HANDLE hThreadArray[ClientList::MAXUSERNUM];
    while (1)
    {
        SOCKET clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);
        //收到客户端的连接，创建子线程处理消息
        if (INVALID_SOCKET == clntSock)
        {
            printf("socket Failed\n");
            continue;
        }
        int clientIndex = ClientList::addClient(clntSock, clntAddr);
        if (clientIndex >= 0)
        {
            PMYDATA pData = new MYDATA(clntSock, (size_t)clientIndex);
            hThreadArray[clientIndex] = CreateThread(NULL, 0, clientFunc, pData, 0, NULL);
        }

        else
        {
            const char* welcomeStr = "Sorry, there are too many users now! please connect later.";
            ClientList::sendMessage(clientIndex, welcomeStr, strlen(welcomeStr) + sizeof(char));
            closesocket(clntSock);
        }
    }
    WaitForMultipleObjects(ClientList::MAXUSERNUM, hThreadArray, TRUE, INFINITE);
    for (size_t i = 0; i < ClientList::MAXUSERNUM; i++)
    {
        CloseHandle(hThreadArray[i]);
    }
    // CloseHandle(hThreadArray[i]);
    closesocket(servSock);
    return 0;
}

DWORD WINAPI clientFunc(LPVOID lpParameter)
{
    PMYDATA pData = (PMYDATA)lpParameter;
    SOCKET clntSock = pData->clntSock;

    //向客户端发送数据
    const char* welcomeStr = "Hello World!";
    send(clntSock, welcomeStr, strlen(welcomeStr) + sizeof(char), NULL);

    //接受客户端数据
    std::string request = ClientList::recvRequest(pData->index);

    //输出收到的数据
    printf("Message from client: %s\n", request.c_str());

    ClientList::removeClient(pData->index);
    closesocket(clntSock);
    return 0;
}