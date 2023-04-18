#include"TCPSocket.h"
class TCPServer
{
public:
    TCPServer();
    ~TCPServer();
    int setListen(unsigned short port);
    TCPSocket* acceptConn(struct sockaddr_in* addr = nullptr);

private:
    int m_fd;	// 监听的套接字
};