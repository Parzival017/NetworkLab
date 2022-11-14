//
// Created by Parzival on 2022/11/11.
//

#ifndef MYFTTPCLIENT_MYLOGGER_H
#define MYFTTPCLIENT_MYLOGGER_H
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

class MyLogger {
private:
    QFile file;
    QTextStream out;
public:
    void log(QString msg);
    MyLogger();
    ~MyLogger();
};


#endif //MYFTTPCLIENT_MYLOGGER_H
