//
// Created by Parzival on 2022/10/20.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ClientWidget.h" resolved

#include "clientwidget.h"
#include "ui_ClientWidget.h"


ClientWidget::ClientWidget(QWidget *parent) :
        QWidget(parent), ui(new Ui::ClientWidget) {
    ui->setupUi(this);
    logger = new MyLogger();
    connect(ui->choose_pushButton, &QPushButton::clicked, this, &ClientWidget::chooseFile);
    connect(ui->send_pushButton, &QPushButton::clicked, this, &ClientWidget::sendFile);
    connect(ui->receive_pushButton, &QPushButton::clicked, this, &ClientWidget::receiveFile);
}

ClientWidget::~ClientWidget() {
    delete logger;
    delete ui;
}



void ClientWidget::chooseFile() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("All Files (*)"));
    ui->send_lineEdit->setText(fileName);
}

void ClientWidget::sendFile() {

    tftpClient = new TFTPClient(logger);
    QString ip = ui->IP_lineEdit->text();
    QString fileName = ui->send_lineEdit->text();
    tftpClient->sendFile(fileName.toStdString().c_str(), ip.toStdString().c_str());
    delete tftpClient;
}

void ClientWidget::receiveFile() {
    tftpClient = new TFTPClient(logger);
    QString ip = ui->IP_lineEdit->text();
    QString fileName = ui->receive_lineEdit->text();
    tftpClient->receiveFile(fileName.toStdString().c_str(), ip.toStdString().c_str());
    delete tftpClient;
}

