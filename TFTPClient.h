//
// Created by Parzival on 2022/10/27.
//

#ifndef MYFTTPCLIENT_TFTPCLIENT_H
#define MYFTTPCLIENT_TFTPCLIENT_H

#include <winsock2.h>
#include <QDebug>
#include <QFileDialog>
#include <QRandomGenerator>
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

#define SLEEPTIME 10
#define TIMEOUT 3000

class TFTPClient {
private:
    WSADATA wsaData;
    SOCKET sock;              //客户端socket
    sockaddr_in serverAddr;   //服务端IP
    sockaddr_in clientAddr;   //客户端IP
    //收发包缓冲区
    char recvBuffer[MAX_BUFFER],sendBuffer[MAX_BUFFER];
    QRandomGenerator randomGenerator;   //随机数产生器
    MyLogger *logger;                   //日志记录器
    long bytesTransferred;   //已传输字节数
public:
    TFTPClient(MyLogger *mylogger){
        // Initialize Winsock
        int nRC = WSAStartup(0x0101, &wsaData);
        if (nRC)
        {
            qDebug() << "客户端winsock出错!";
            return;
        }
        if (wsaData.wVersion != 0x0101)
        {
            qDebug() << "客户端winsock版本有误!";
            WSACleanup();
            return;
        }
        //为随机数产生器分配种子
        randomGenerator = QRandomGenerator::securelySeeded();
        logger = mylogger;
    };
    ~TFTPClient(){
        WSACleanup();
    };
    void start(const char* serverIP);
    void stop();
    void uploadFile(const char* fileName);
    void downloadFile(const char* fileName);
    void sendFile(const char* fileName,const char * serverIP);
    void receiveFile(const char* fileName,const char * serverIP);
};


#endif //MYFTTPCLIENT_TFTPCLIENT_H
