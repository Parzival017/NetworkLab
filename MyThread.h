//
// Created by Parzival on 2022/11/15.
//

#ifndef MYFTTPCLIENT_MYTHREAD_H
#define MYFTTPCLIENT_MYTHREAD_H
#include <QThread>
#include <winsock2.h>
#include <QDebug>
#include <QFileDialog>
#include <QRandomGenerator>
#include <QElapsedTimer>
#include <QProgressDialog>
#include "MyLogger.h"


#define SERVER_PORT 69     //TFTP服务端口号
#define MAX_BUFFER 1024    //缓冲区长度
#define MAX_DATA 512       //单个数据块长度
#define MAX_FILENAME 512   //最大文件名长度

//TFTP操作码
#define RRQ 1
#define WRQ 2
#define DATA 3
#define ACK 4
#define TFTP_ERROR 5
#define LIST 6
#define HEAD 7

//最大重传次数
#define MAX_RESEND_COUNT 3

#define TIMEOUT 3

#define RET_SUCCESS 0
#define RET_TIMEOUT 1
#define RET_ERROR 2

#define THREAD_SEND 1
#define THREAD_RECEIVE 2

class MyThread : public QThread {
    Q_OBJECT
private:
    QString fileName;
    QString ip;
    int type;
    WSADATA wsaData;
    SOCKET sock;              //客户端socket
    sockaddr_in serverAddr;   //服务端IP
    sockaddr_in clientAddr;   //客户端IP
    //收发包缓冲区
    char recvBuffer[MAX_BUFFER],sendBuffer[MAX_BUFFER];
    QRandomGenerator randomGenerator;   //随机数产生器
    MyLogger *logger;                   //日志记录器
    struct timeval tv;
    fd_set readfds;

public:
    MyThread(){};
    bool init(QString fileName,QString ip, int type, MyLogger *logger);
    bool startNet(QString serverIP);
    bool stopNet();
    void uploadFile(const char* fileName);
    void downloadFile(const char* fileName);
    void sendFile(QString fileName,QString serverIP);
    void receiveFile(QString fileName,QString serverIP);
signals:
    //传输速度信号
    void updateSpeed(float aveSpeed, float curSpeed);
    //日志信号
    void updateMsg(QString msg);
protected:
    void run();
};




#endif //MYFTTPCLIENT_MYTHREAD_H
