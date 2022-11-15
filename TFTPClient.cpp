//
// Created by Parzival on 2022/10/27.
//




#include "TFTPClient.h"

bool TFTPClient::start(const char * serverIP) {
    // 初始化套接字
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(serverIP);
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock == INVALID_SOCKET){
        int error = WSAGetLastError();
        logger->log("socket() failed with error code : " + QString::number(error));
        return false;
    }

    // 随机产生客户端端口号并绑定
    for (int i=0;i<1024;i++) {
        int port = randomGenerator.bounded(1024, 65535);
        clientAddr.sin_family = AF_INET;
        clientAddr.sin_port = htons(port);
        clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        if (bind(sock, (sockaddr *) &clientAddr, sizeof(clientAddr)) == SOCKET_ERROR) {
            int error = WSAGetLastError();
            qDebug() << "bind() failed with error code : " + QString::number(error);
        }else{
            break;
        }
    }
    if(sock == INVALID_SOCKET){
        int error = WSAGetLastError();
        logger->log("bind() failed with error code : " + QString::number(error));
        return false;
    }

    logger->log("Started socket: "+QString::number(sock)+" on port: "+QString::number(ntohs(clientAddr.sin_port))+" serverIP: "+QString(serverIP));

    //清空缓冲区
    memset(recvBuffer, 0, MAX_BUFFER);
    memset(sendBuffer, 0, MAX_BUFFER);

    return true;
}

bool TFTPClient::stop() {
    closesocket(sock);
    logger->log("Stopped socket: "+QString::number(sock));
    return true;
}

void TFTPClient::uploadFile(const char *orifileName,struct returnData* retData) {
    logger->log("Uploading file: "+QString(orifileName));

    //计时
    QElapsedTimer timer;
    timer.start();

    int resendCount; //重传次数
    long successCount = 0; //成功发送的数据块数
    long lossCount = 0; //丢失的数据块数
    long bytesTransferred = 0; //已传输的字节数

    //打开文件
    FILE *fp = fopen(orifileName, "rb");
    if(fp == nullptr){
        logger->log("Upload:File not found: "+QString(orifileName));
        retData->code = RET_ERROR;
        retData->msg = "File not found: "+QString(orifileName);
        return;
    }

    //构造WRQ包
    sendBuffer[0] = 0;
    sendBuffer[1] = WRQ;
    //去除路径，仅保留文件名
    char fileName[MAX_FILENAME];
    strcpy(fileName, QString(orifileName).split("/").last().toStdString().c_str());
    //将文件名写入缓冲区
    strcpy(sendBuffer + 2, fileName);
    //将文件模式写入缓冲区
    strcpy(sendBuffer + 2 + strlen(fileName) + 1, "octet");
    int len = 2 + strlen(fileName) + 1 + strlen("octet") + 1;

    //发送WRQ包
    for(resendCount = 0;resendCount < MAX_RESEND_COUNT;resendCount++){
        int ret = sendto(sock, sendBuffer, len, 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
        if(ret == SOCKET_ERROR){
            int error = WSAGetLastError();
            logger->log("Upload:WRQ:sendto() failed with error code : " + QString::number(error));
            continue;
        } else successCount++;

        //接收ACK包
        // 超时处理
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);
        tv.tv_sec = TIMEOUT;
        tv.tv_usec = 0;
        select(sock + 1, &readfds, NULL, NULL, &tv);

        if(FD_ISSET(sock, &readfds)) {
            // 尝试接收
            int serverAddrLen = sizeof(serverAddr);
            int recvSize = recvfrom(sock, recvBuffer, MAX_BUFFER, 0, (sockaddr *) &serverAddr, &serverAddrLen);
            if (recvSize > 0) {
                //判断是否为ACK包
                if (recvBuffer[1] == ACK) {
                    //判断ACK包是否为期望的ACK包
                    if (recvBuffer[2] == 0 && recvBuffer[3] == 0) {
                        //ACK包正确
                        successCount++;
                        break;
                    } else {
                        //ACK包错误
                        qDebug() << "ACK error";
                        continue;
                    }
                } else {
                    //不是ACK包
                    qDebug() << "Not ACK";
                    continue;
                }
            }
        }
        else{
            //超时,重传WRQ包
            lossCount++;
            logger->log("Upload:WRQ:Timeout, resend WRQ");
            continue;
        }
    }
    if(resendCount>=MAX_RESEND_COUNT){
        //重传次数超过最大重传次数
        logger->log("Upload:WRQ:Resend count exceeded maximum resend count");
        retData->code = RET_TIMEOUT;
        retData->msg = "Resend count exceeded maximum resend count";
        return;
    }

    //发送数据
    unsigned short blockNum = 1;
    int readSize = 0;
    do{
        memset(sendBuffer, 0, MAX_BUFFER);
        readSize = fread(sendBuffer + 4, 1, 512, fp);
        sendBuffer[0] = 0;
        sendBuffer[1] = DATA;
        sendBuffer[2] = blockNum / 256;
        sendBuffer[3] = blockNum % 256;    //Block Number

        //发送数据包
        for(resendCount = 0;resendCount < MAX_RESEND_COUNT;resendCount++){
            if(sendto(sock, sendBuffer, readSize + 4, 0, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR){
            int error = WSAGetLastError();
            logger->log("Upload:DATA:sendto() failed with error code : " + QString::number(error));
            continue;
            }
            else successCount++;

            //接收ACK包
            // 等待接收回应,最多等待3s,每隔20ms刷新一次
            // 超时处理
            FD_ZERO(&readfds);
            FD_SET(sock, &readfds);
            tv.tv_sec = TIMEOUT;
            tv.tv_usec = 0;
            select(sock + 1, &readfds, NULL, NULL, &tv);

            if(FD_ISSET(sock, &readfds)) {

                // 尝试接收
                int serverAddrLen = sizeof(serverAddr);
                memset(recvBuffer, 0, MAX_BUFFER);
                recvfrom(sock, recvBuffer, MAX_BUFFER, 0, (sockaddr *) &serverAddr, &serverAddrLen);
                //收到ERROR包
                if(recvBuffer[1] == TFTP_ERROR){
                    logger->log("Download:ERROR:Error code: " + QString::number((unsigned char)recvBuffer[3]) + " Error message: " + QString::fromLocal8Bit(recvBuffer+4));
                    retData->code = RET_ERROR;
                    retData->msg = "Download:ERROR:Error code: " + QString::number((unsigned char)recvBuffer[3]) + " Error message: " + QString::fromLocal8Bit(recvBuffer+4);
                    fclose(fp);
                    return;
                }
                WSAGetLastError();
                unsigned int blockNumRecv = (unsigned char)recvBuffer[2] * 256 + (unsigned char)recvBuffer[3];
                if (recvBuffer[1] == ACK && blockNumRecv == blockNum) {
                    //ACK包正确
                    successCount++;
                    break;
                }

            }
            else{
                //超时,重传数据包
                lossCount++;
                qDebug() << "ACK timeout :" << blockNum;
                logger->log("Upload:DATA:Timeout, resend DATA");
                continue;
            }
        }

        if(resendCount>=MAX_RESEND_COUNT){
            //重传次数超过最大重传次数
            logger->log("Upload:DATA:Timeout:Resend count exceeded maximum resend count");
            retData->code = RET_TIMEOUT;
            retData->msg = "Resend count exceeded maximum resend count";
            return;
        }
        blockNum++;
        if(blockNum == 65536) blockNum = 0;
        bytesTransferred += readSize;
    }while (readSize == MAX_DATA);	// 当数据块未装满时认为时最后一块数据，结束循环

    fclose(fp);
    double timeCost = (1.0*timer.elapsed())/1000;
    QString retMsg = "Upload:File:"+ QString::fromLocal8Bit(fileName) + " uploaded successfully, " + QString::number(bytesTransferred) + " bytes transferred in " + QString::number(timeCost) + " seconds, " + QString::number(1.0*bytesTransferred/timeCost/1e6) + " Mb/s";
    logger->log(retMsg);
    retData->code = RET_SUCCESS;
    retData->msg = retMsg;
    return;
}

void TFTPClient::downloadFile(const char *fileName,struct returnData* retData) {
    logger->log("Downloading file: "+QString(fileName));
    //计时
    QElapsedTimer timer;
    timer.start();

    long successCount = 0; //成功发送的数据块数
    long lossCount = 0; //丢失的数据块数
    long bytesTransferred = 0; //传输的字节数

    FILE *fp = fopen(fileName, "wb");
    if(fp == nullptr){
        logger->log("Download:File:"+ QString::fromLocal8Bit(fileName) + " create failed");
        retData->code = RET_ERROR;
        retData->msg = "Download:File:"+ QString::fromLocal8Bit(fileName) + " create failed";
        return;
    }

    //发送RRQ
    sendBuffer[0] = 0;
    sendBuffer[1] = RRQ;
    strcpy(sendBuffer + 2, fileName);
    strcpy(sendBuffer + 2 + strlen(fileName) + 1, "octet");
    int len = 2 + strlen(fileName) + 1 + strlen("octet") + 1;

    if(sendto(sock, sendBuffer, len, 0, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR){
        int error = WSAGetLastError();
        logger->log("Download:RRQ:sendto() failed with error code : " + QString::number(error));
        retData->code = RET_ERROR;
        retData->msg = "Download:RRQ:sendto() failed with error code : " + QString::number(error);
        return;
    }
    else successCount++;

    //接收数据
    int recvSize = 0;
    int blockNum = 1;

    do{
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);
        tv.tv_sec = TIMEOUT;
        tv.tv_usec = 0;
        select(sock + 1, &readfds, NULL, NULL, &tv);
        int resendCount = 0;
        for(resendCount = 0;resendCount < MAX_RESEND_COUNT;resendCount++){
            if(FD_ISSET(sock, &readfds)) {
                // 尝试接收
                int serverAddrLen = sizeof(serverAddr);
                memset(recvBuffer, 0, MAX_BUFFER);
                recvSize = recvfrom(sock, recvBuffer, MAX_BUFFER, 0, (sockaddr *) &serverAddr, &serverAddrLen);
                //收到ERROR包
                if(recvBuffer[1] == TFTP_ERROR){
                    logger->log("Download:ERROR:Error code: " + QString::number((unsigned char)recvBuffer[3]) + " Error message: " + QString::fromLocal8Bit(recvBuffer+4));
                    retData->code = RET_ERROR;
                    retData->msg = "Download:ERROR:Error code: " + QString::number((unsigned char)recvBuffer[3]) + " Error message: " + QString::fromLocal8Bit(recvBuffer+4);
                    fclose(fp);
                    remove(fileName);
                    return;
                }
                if (recvBuffer[1] == DATA && (unsigned char)recvBuffer[2] * 256 + (unsigned char)recvBuffer[3] == blockNum) {
                    //数据包正确
                    successCount++;
                    //发送ACK
                    sendBuffer[0] = 0;
                    sendBuffer[1] = ACK;
                    sendBuffer[2] = blockNum / 256;
                    sendBuffer[3] = blockNum % 256;    //Block Number
                    sendto(sock, sendBuffer, 4, 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
                    //写入文件
                    fwrite(recvBuffer + 4, 1, recvSize - 4, fp);
                    break;
                }else{
                    //数据包错误
                    logger->log("Download:DATA:Error, resend ACK");
                    continue;
                }
            }
            else{
                qDebug() << "DATA timeout :" << blockNum;
                if(blockNum == 1){
                    logger->log("Download:DATA:Timeout:No data received");
                    retData->code = RET_TIMEOUT;
                    retData->msg = "Download:DATA:Timeout:No data received";
                    //关闭文件
                    fclose(fp);
                    //删除文件
                    remove(fileName);
                    return;
                }
                if(blockNum > 1 && resendCount < MAX_RESEND_COUNT){
                    //超时,重传ACK
                    lossCount++;
                    logger->log("Download:DATA:Timeout:Resend ACK");
                    resendCount++;
                    sendBuffer[0] = 0;
                    sendBuffer[1] = ACK;
                    sendBuffer[2] = (blockNum-1) / 256;
                    sendBuffer[3] = (blockNum-1) % 256;    //Block Number
                    sendto(sock, sendBuffer, 4, 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
                    recvSize = MAX_DATA + 4; //避免跳出循环
                    continue;
                }
                if(resendCount == MAX_RESEND_COUNT){
                    //重传次数超过最大重传次数
                    logger->log("Download:DATA:Timeout:Resend count exceeded maximum resend count");
                    fclose(fp);
                    retData->code = RET_TIMEOUT;
                    retData->msg = "Download:DATA:Timeout:Resend count exceeded maximum resend count";
                    //关闭文件
                    fclose(fp);
                    //删除文件
                    remove(fileName);
                    return;
                }
            }
        }


        bytesTransferred += recvSize - 4;
        blockNum++;
        if(blockNum == 65536) blockNum = 0;
    }while(recvSize == MAX_DATA + 4);	// 当数据块未装满时认为时最后一块数据，结束循环

    //关闭文件
    fclose(fp);
    double timeCost = (1.0*timer.elapsed())/1000;
    QString retMessage = "Download:File:"+ QString::fromLocal8Bit(fileName) + " downloaded successfully, " + QString::number(bytesTransferred) + " bytes transferred in " + QString::number(timeCost) + " seconds, " + QString::number(1.0*bytesTransferred/timeCost/1e6) + " Mb/s" ;
    logger->log(retMessage);
    retData->code = RET_SUCCESS;
    retData->msg = retMessage;
    return;
}

struct returnData TFTPClient::sendFile(const char* fileName,const char * serverIP){
    struct returnData* data = new struct returnData;

    //初始化
    if(!start(serverIP)){
        data->code = RET_ERROR;
        data->msg = "Send:Initialization failed";
        return *data;
    }
    //发送文件
    uploadFile(fileName, data);
    //关闭连接
    stop();
    return *data;
}

struct returnData TFTPClient::receiveFile(const char* fileName,const char * serverIP){
    struct returnData* data = new struct returnData;
    //初始化
    if(!start(serverIP)){
        data->code = RET_ERROR;
        data->msg = "Receive:Initialization failed";
        return *data;
    }
    //接收文件
    downloadFile(fileName,data);
    //关闭连接
    stop();
    return *data;
}