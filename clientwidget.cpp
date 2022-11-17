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

    QString ip = ui->IP_lineEdit->text();
    QString fileName = ui->send_lineEdit->text();
    MyThread *thread = new MyThread();
    thread->init(fileName, ip, THREAD_SEND, logger);
    connect(thread, SIGNAL(updateMsg(QString)), this, SLOT(displayMsg(QString)));
    //弹出一对话框显示文件名及传输速度
    MyDialog *dialog = new MyDialog();
    dialog->setWindowTitle("Recerving" + fileName);
    connect(thread, SIGNAL(updateSpeed(float, float)), dialog, SLOT(displaySpeed(float, float)));
    dialog->show();
    thread->start();
    //进程结束后，关闭对话框
    connect(thread, SIGNAL(finished()), dialog, SLOT(close()));
}

void ClientWidget::receiveFile() {

    QString ip = ui->IP_lineEdit->text();
    QString fileName = ui->receive_lineEdit->text();

    MyThread *thread = new MyThread();
    thread->init(fileName, ip, THREAD_RECEIVE, logger);
    connect(thread, SIGNAL(updateMsg(QString)), this, SLOT(displayMsg(QString)));
    //弹出一对话框显示文件名及传输速度
    MyDialog *dialog = new MyDialog();
    dialog->setWindowTitle("Recerving" + fileName);
    connect(thread, SIGNAL(updateSpeed(float, float)), dialog, SLOT(displaySpeed(float, float)));
    dialog->show();
    thread->start();
    //进程结束后，关闭对话框
    connect(thread, SIGNAL(finished()), dialog, SLOT(close()));
}

void ClientWidget::displayMsg(QString msg) {

    ui->textBrowser->append(msg);
}



