#include "client.h"
#include "../protocol/my_protocol.hpp"

#pragma warning(disable:4996)

int Client::start()
{
    //创建套接字
    servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    //向服务器发起请求
    sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = PF_INET;
    sockAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    sockAddr.sin_port = htons(1730);
    connect(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));


    //接受服务器传回数据
    std::string str = MyProtocol::recvMessage(servSock);
    printf("%s", str.c_str());

    char name[] = "I am client";
    std::string msg;
    std::getline(std::cin, msg);
    MyProtocol::sendMessage(servSock, msg.c_str(), msg.size() + sizeof(char));

    closesocket(servSock);
    return 0;
}
