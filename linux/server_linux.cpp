#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main()
{
    //创建套接字
    int servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    //绑定套接字
    sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family=PF_INET;
    sockAddr.sin_addr.s_addr=inet_addr("192.168.1.152");
    sockAddr.sin_port = htons(1730);
    bind(servSock,(sockaddr*)&sockAddr, sizeof(sockaddr));

    //监听
    listen(servSock,20);

    //接受客户端请求
    sockaddr clntAddr;
    socklen_t nSize = sizeof(sockaddr);
    int clntSock = accept(servSock,(sockaddr*)&clntAddr, &nSize);

    //向客户端发送数据
    const char str[] = "Hello World!";
    write(clntSock,str,sizeof(str));

    close(clntSock);
    close(servSock);

    return 0;
}