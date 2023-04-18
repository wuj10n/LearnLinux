#ifndef TCPSOCKET_H
#define TCPSOCKET_H
#include<string.h>
class TCPSocket
{
public:
    TCPSocket();
    TCPSocket(int socket);
    ~TCPSocket();
    int connectToHost(string ip, unsigned short port);
    int sendMsg(string msg);
    string recvMsg();

private:
    int readn(char* buf, int size);
    int writen(const char* msg, int size);

private:
    int m_fd;	// 通信的套接字
};
#endif
