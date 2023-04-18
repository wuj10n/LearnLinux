#include<string.h>
class TCPClient
{
public:
    TCPClient();
    ~TCPClient();
    // int connectToHost(int fd, const char* ip, unsigned short port);
    int connectToHost(string ip, unsigned short port);

    // int sendMsg(int fd, const char* msg);
    int sendMsg(string msg);
    // int recvMsg(int fd, char* msg, int size);
    string recvMsg();
    
    // int createSocket();
    // int closeSocket(int fd);

private:
    // int readn(int fd, char* buf, int size);
    int readn(char* buf, int size);
    // int writen(int fd, const char* msg, int size);
    int writen(const char* msg, int size);
    
private:
    int cfd;	// 通信的套接字
};