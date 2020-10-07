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

    //向服务器发起请求
    sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family=PF_INET;
    sockAddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    sockAddr.sin_port = htons(1730);
    connect(servSock,(sockaddr*)&sockAddr, sizeof(sockaddr));

    //接受服务器传回数据
    char buffer[40]={0};
    read(servSock, buffer, sizeof(buffer)-1);

    //输出收到的数据
    printf("Message from server: %s\n",buffer);

    close(servSock);
    return 0;
}