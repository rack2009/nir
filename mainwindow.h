#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <newfile.h>
#include <adduser.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    Ui::MainWindow *ui;

private slots:
    void connect_db();
    void slotFinished(int a);
    void on_tableView_doubleClicked(const QModelIndex &index);
    void update();
    void recieveData(QString str);
    void recieveDatafromAddUser(QString username, QString password);
    void on_actionOpen_triggered();
    void on_actionRefresh_triggered();
    void on_actionFile_information_triggered();
    void pipeReader(QString path);

protected:
    virtual void closeEvent(QCloseEvent *e);

private:
    NewFile *newfile;
    AddUser *adduser;

};

#endif // MAINWINDOW_H
