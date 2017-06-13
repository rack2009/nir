#include "newfile.h"
#include "ui_newfile.h"

#include <QtSql/QSqlError>
#include <QDebug>
#include <QtSql/QSqlDatabase>
#include <QtGui>
#include <QTableView>
#include <QMessageBox>
#include <QString>
#include <QSqlQueryModel>
#include <QSqlTableModel>

NewFile::NewFile(QWidget *parent) :
    QDialog(parent),
    ui1(new Ui::NewFile)
{
    ui1->setupUi(this);
}

NewFile::~NewFile()
{
    delete ui1;
}

void NewFile::on_buttonBox_accepted()
{
    if (ui1->lineEdit->text() == NULL)
    {
        QMessageBox errmsg;

        errmsg.setWindowTitle("Error");
        errmsg.setInformativeText("You must specify a file name");
        errmsg.setStandardButtons(QMessageBox::Cancel);
        errmsg.exec();

        NewFile::show();
    }
    else
    {
        emit sendData(ui1->lineEdit->text());
    }
    qDebug() << QObject::trUtf8("Ok Pressed");
}

void NewFile::on_buttonBox_rejected()
{
    qDebug() << QObject::trUtf8("Cancel Pressed");
}
