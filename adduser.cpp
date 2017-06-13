#include "adduser.h"
#include "ui_adduser.h"

#include <QMessageBox>
#include <QString>
#include <QDebug>

AddUser::AddUser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddUser)
{
    ui->setupUi(this);
}

AddUser::~AddUser()
{
    delete ui;
}

void AddUser::on_buttonBox_accepted()
{
    if (ui->lineEdit->text() == NULL || ui->lineEdit_2->text() == NULL)
    {
        QMessageBox errmsg;

        errmsg.setWindowTitle("Error");
        errmsg.setInformativeText("You must specify a username and password");
        errmsg.setStandardButtons(QMessageBox::Cancel);
        errmsg.exec();

        AddUser::show();
    }
    else
    {
        emit sendData(ui->lineEdit->text(), ui->lineEdit_2->text());
    }
    qDebug() << QObject::trUtf8("Ok Pressed");
}
