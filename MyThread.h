//
// Created by Parzival on 2022/11/15.
//

#ifndef MYFTTPCLIENT_MYTHREAD_H
#define MYFTTPCLIENT_MYTHREAD_H
#include <QThread>
#include "MyLogger.h"
#include "TFTPClient.h"
#define THREAD_SEND 1
#define THREAD_RECEIVE 2

class MyThread : public QThread {
    Q_OBJECT
public:
    MyThread(){};
    void init(const char *fileName,const char *ip, int type, MyLogger *logger);

private:
    const char *fileName;
    const char *ip;
    int type;
    MyLogger *logger;
    TFTPClient *tftpClient;
    struct returnData data;
protected:
    void run();
};




#endif //MYFTTPCLIENT_MYTHREAD_H
