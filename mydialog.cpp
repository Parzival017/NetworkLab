//
// Created by Parzival on 2022/11/15.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MyDialog.h" resolved

#include "mydialog.h"
#include "ui_MyDialog.h"


MyDialog::MyDialog(QWidget *parent) :
        QDialog(parent), ui(new Ui::MyDialog) {
    ui->setupUi(this);
}

MyDialog::~MyDialog() {
    delete ui;
}

void MyDialog::displaySpeed(float aveSpeed, float curSpeed) {
    ui->speed_label->setText("Current: "+ QString::number(curSpeed) +" Mb/s Average: "+ QString::number(aveSpeed) +" Mb/s");
}

