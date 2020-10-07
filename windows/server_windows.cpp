#include <stdio.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib") //加载 ws2_32.dll
#pragma warning(disable:4996)

int main()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    //创建套接字
    SOCKET servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    //绑定套接字
    sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family=PF_INET;
    sockAddr.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");
    sockAddr.sin_port = htons(1730);
    bind(servSock,(SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

    //监听
    listen(servSock,20);

    //接受客户端请求
    SOCKADDR clntAddr;
    int nSize = sizeof(SOCKADDR);
    SOCKET clntSock = accept(servSock,(SOCKADDR*)&clntAddr, &nSize);

    //向客户端发送数据
    const char* str = "Hello World!";
    send(clntSock,str,strlen(str)+sizeof(char),NULL);

    closesocket(clntSock);
    closesocket(servSock);

    WSACleanup();

    return 0;
}