//
// Created by Parzival on 2022/11/15.
//

#include "MyThread.h"

void MyThread::init(const char *fileName,const char *ip, int type, MyLogger *logger) {
    this->fileName = fileName;
    this->ip = ip;
    this->type = type;
    this->logger = logger;
    tftpClient = new TFTPClient(logger);

}

void MyThread::run() {
    if (type == THREAD_SEND) {
        data = tftpClient->sendFile(fileName, ip);
    } else if (type == THREAD_RECEIVE) {
        data = tftpClient->receiveFile(fileName, ip);
    }
    delete tftpClient;
}
