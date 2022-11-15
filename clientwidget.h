//
// Created by Parzival on 2022/10/20.
//

#ifndef MYFTTPCLIENT_CLIENTWIDGET_H
#define MYFTTPCLIENT_CLIENTWIDGET_H

#include <QWidget>
#include <QThread>
#include "TFTPClient.h"
#include "MyLogger.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ClientWidget; }
QT_END_NAMESPACE

class ClientWidget : public QWidget {
Q_OBJECT

public:
    explicit ClientWidget(QWidget *parent = nullptr);

    ~ClientWidget() override;
    void sendFile();
    void receiveFile();
    void chooseFile();
    void displayLog(QString log);
    void makeLog(struct returnData* data);

private:
    MyLogger *logger;
    TFTPClient tftpClient;
    Ui::ClientWidget *ui;
};



#endif //MYFTTPCLIENT_CLIENTWIDGET_H
