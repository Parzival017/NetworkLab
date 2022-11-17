//
// Created by Parzival on 2022/11/15.
//

#ifndef MYFTTPCLIENT_MYDIALOG_H
#define MYFTTPCLIENT_MYDIALOG_H

#include <QDialog>


QT_BEGIN_NAMESPACE
namespace Ui { class MyDialog; }
QT_END_NAMESPACE

class MyDialog : public QDialog {
Q_OBJECT

public:
    explicit MyDialog(QWidget *parent = nullptr);

    ~MyDialog() override;

private:
    Ui::MyDialog *ui;
public slots:
    void displaySpeed(float aveSpeed,float curSpeed);
};


#endif //MYFTTPCLIENT_MYDIALOG_H
