

int Client::start()
{
    //�����׽���
    SOCKET servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    //���������������
    sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = PF_INET;
    sockAddr.sin_addr.S_un.S_addr = inet_addr("192.168.1.152");
    sockAddr.sin_port = htons(1730);
    connect(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

    //���ܷ�������������
    char szBuffer[MAXBYTE] = { 0 };
    recv(servSock, szBuffer, MAXBYTE, NULL);

    //����յ�������
    printf("Message from server: %s\n", szBuffer);

    closesocket(servSock);
}