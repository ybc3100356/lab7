#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib") //加载 ws2_32.dll
#pragma warning(disable:4996)

int main()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    //创建套接字
    SOCKET servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    //向服务器发起请求
    sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family=PF_INET;
    sockAddr.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");
    sockAddr.sin_port = htons(1730);
    connect(servSock,(SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

    //接受服务器传回数据
    char szBuffer[MAXBYTE]={0};
    recv(servSock, szBuffer, MAXBYTE, NULL);

    //输出收到的数据
    printf("Message from server: %s\n",szBuffer);

    closesocket(servSock);

    WSACleanup();

    system("pause");
    return 0;
}