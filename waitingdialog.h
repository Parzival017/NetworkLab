//
// Created by Parzival on 2022/11/12.
//

#ifndef MYFTTPCLIENT_WAITINGDIALOG_H
#define MYFTTPCLIENT_WAITINGDIALOG_H

#include <QDialog>
#include <QMovie>

QT_BEGIN_NAMESPACE
namespace Ui { class WaitingDialog; }
QT_END_NAMESPACE

class WaitingDialog : public QDialog {
Q_OBJECT

public:
    explicit WaitingDialog(QWidget *parent = nullptr);
    QMovie *myMovie;
    ~WaitingDialog() override;

private:
    Ui::WaitingDialog *ui;
};


#endif //MYFTTPCLIENT_WAITINGDIALOG_H
