//
// Created by Parzival on 2022/11/11.
//

#include "MyLogger.h"

MyLogger::MyLogger() {
    file.setFileName("log.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
    out.setDevice(&file);
    out << "**********************************************************\n";
    out << "                  MyLogger initialized                    \n";
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz");
    out << "               "+current_date_time+"                    \n";
    out << "**********************************************************\n";
}

MyLogger::~MyLogger() {
    out << "**********************************************************\n";
    out << "                  MyLogger destroyed                     \n";
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:zzz");
    out << "               "+current_date_time+"                    \n";
    out << "**********************************************************\n";
    file.close();
}

void MyLogger::log(QString msg) {
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:zzz");
    out << current_date_time+" "+msg+"\n";
}
