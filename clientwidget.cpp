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
    //创建等待窗口
    QProgressDialog *progressDialog = new QProgressDialog(this);
    progressDialog->setWindowTitle("Please wait:Sending...");
    //不显示进度条和取消按钮，仅显示label
    progressDialog->setLabelText(QLabel::tr("Sending..."));
    progressDialog->setMinimumDuration(0);
    progressDialog->show();



    tftpClient = new TFTPClient(logger);
    QString ip = ui->IP_lineEdit->text();
    QString fileName = ui->send_lineEdit->text();
    struct returnData data = tftpClient->sendFile(fileName.toStdString().c_str(), ip.toStdString().c_str());
    makeLog(&data);
    delete tftpClient;

    progressDialog->close();
}

void ClientWidget::receiveFile() {



    tftpClient = new TFTPClient(logger);
    QString ip = ui->IP_lineEdit->text();
    QString fileName = ui->receive_lineEdit->text();
    struct returnData data = tftpClient->receiveFile(fileName.toStdString().c_str(), ip.toStdString().c_str());
    QDateTime time = QDateTime::currentDateTime();
    makeLog(&data);
    delete tftpClient;

//    MyThread *thread = new MyThread();
//    thread->init(fileName.toStdString().c_str(), ip.toStdString().c_str(), THREAD_RECEIVE, logger);
//    thread->start();

}

void ClientWidget::displayLog(QString log) {

    ui->textBrowser->append(log);
}

void ClientWidget::makeLog(struct returnData* data) {
    QDateTime time = QDateTime::currentDateTime();
    QString current_time = time.toString("yyyy-MM-dd hh:mm:ss");
    QString log;
    if(data->code == RET_SUCCESS)
        log = current_time + " SUCCESS " + data->msg;
    else if(data->code == RET_ERROR)
        log = current_time + " ERROR " + data->msg;
    else
        log = current_time + " TIMEOUT " + data->msg;
    displayLog(log);
}
