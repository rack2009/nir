#include "mainwindow.h"
#include "newfile.h"
#include "ui_mainwindow.h"
#include "ui_newfile.h"
#include "properties.h"

#include <QtSql/QSqlError>
#include <QDebug>
#include <QtSql/QSqlDatabase>
#include <QtGui>
#include <QTableView>
#include <QMessageBox>
#include <QString>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QtConcurrent/QtConcurrent>

#include <unistd.h>
#include <fly/flyhelpmenu.h>

QList<QProcess *> processes;
QStringList list;
QStringList PIDs;
QDir * dir = new QDir("/proc");
QString username = qgetenv("USER");
QDir * docdir = new QDir("/home/acl_test/storage/" + username);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->showFullScreen();
    newfile = new NewFile;
    adduser = new AddUser;

    connect(newfile, SIGNAL(sendData(QString)), this, SLOT(recieveData(QString)));
    connect(adduser, SIGNAL(sendData(QString,QString)), this, SLOT(recieveDatafromAddUser(QString,QString)));
    connect(ui->actionToolInsert, SIGNAL(triggered(bool)), newfile, SLOT(show()));
    connect(ui->actionAdd_user, SIGNAL(triggered(bool)), adduser, SLOT(show()));
    connect(ui->actionExit, SIGNAL(triggered(bool)), this, SLOT(close()));

    if (getuid())
    {
        //ui->menuAdministration->hide();
        //ui->menuAdministration->menuAction()->setVisible(false);
        qDebug() << "You must be root!";
    }

    ui->actionToolInsert->setIcon(QIcon::fromTheme("list-add"));
    ui->actionRefresh->setIcon(QIcon::fromTheme("view-refresh"));
    ui->actionExit->setIcon(QIcon::fromTheme("application-exit"));
    ui->actionShare->setIcon(QIcon::fromTheme("go-up"));
    ui->actionOpen->setIcon(QIcon::fromTheme("document-open"));
    setCentralWidget(ui->tableView);
    menuBar()->addMenu(new FlyHelpMenu(this));

    connect_db();


    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);


    for (int c = 0; c < ui->tableView->horizontalHeader()->count(); ++c)
    {
        ui->tableView->horizontalHeader()->setSectionResizeMode(
            c, QHeaderView::Stretch);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connect_db()
{
    if (username.isEmpty())
        username = qgetenv("USERNAME");
    //qDebug() << username;
    //qDebug() << docdir->path();

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setPort(5432);
    db.setDatabaseName("testdb");
    db.setUserName(username);
    //db.setPassword("postgres");
    if (!db.open())
    {
                qDebug() << QObject::trUtf8("test1 :Database not found") << db.lastError().text();
    }
    QSqlTableModel * model = new QSqlTableModel;
    model->setTable("\"DocView\"");

    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    model->removeColumn(0); //hide ID column
    model->setHeaderData(0, Qt::Horizontal, tr("Document name"));
    model->setHeaderData(1, Qt::Horizontal, tr("Document owner"));
    model->setHeaderData(2, Qt::Horizontal, tr("Document path"));
    ui->tableView->setModel(model);
    ui->tableView->setWindowTitle(QObject::trUtf8("SUD"));
    ui->tableView->show();
}

void MainWindow::recieveData(QString str)
{
    QString recieved = str;
    QSqlQueryModel * query = new QSqlQueryModel;
    QProcess *proc = new QProcess;

    QString formedpath = docdir->path() + "/" + recieved + ".odt";
    qDebug() << formedpath;

    proc->start("touch", QStringList() << formedpath);

    query->setQuery("INSERT INTO \"Document\" (id, \"DocName\", username, \"absPath\") VALUES (DEFAULT, \'" + recieved + "\', \'" + username + "\', \'" + formedpath + "\')");

    connect_db();
}

void MainWindow::recieveDatafromAddUser(QString username, QString password)
{
    qDebug() << "username: " + username;
    qDebug() << "password: " + password;

    QProcess *proc = new QProcess;
    QString test = "/home/rack2009/adduser.sh -U " + username + " -P " + password;
    qDebug() << "test: " + test;
    //proc->startDetached( "/bin/bash", QStringList() << "/home/rack2009/adduser.sh -U " + username + " -P " + password);
    proc->start( "/bin/sh", QStringList() << test);
    qDebug() << proc->exitCode();
    qDebug() << proc->readAllStandardOutput();
}

void MainWindow::pipeReader(QString path)
{
    QString pipe_path = path + ".pipe";
    QProcess *reader = new QProcess;
    QString output;
    while (output != "2\n")
    {
        reader->start("cat", QStringList() << pipe_path);
        reader->waitForFinished();
        output = reader->readAllStandardOutput();
        if (output == "2\n")
        {
            reader->kill();
            QProcess *terminate = new QProcess;
            terminate->start("rm", QStringList() << pipe_path);
        }
        //eventCallback
        qDebug() << "Document" << path << "in" << output << "statement";
    }
}


// open file
void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    QModelIndex index1 = ui->tableView->model()->index(ui->tableView->currentIndex().row(),2);
    if (index1.column() == 2)
    {
        QString filePath = ui->tableView->model()->data(index1).toString();
        qDebug(filePath.toStdString().c_str());
        QProcess *process = new QProcess;
        process->start("libreoffice", QStringList() << filePath);

        PIDs.append(QString::number(process->pid()));

        qDebug() << QObject::trUtf8("processId: ") << QString::number(process->pid()).toStdString().c_str();

        update();
    }
}

void MainWindow::slotFinished(int a)
{
    QProcess *process = dynamic_cast<QProcess*>(sender());
    qDebug() << process->readAllStandardOutput();

    int process_id = process->property("PID").toInt();
    qDebug() << QObject::trUtf8("Let`s look what it worth: ") << QString::number(process_id);

}

void MainWindow::update()
{
    list = dir->entryList(QStringList("*"), QDir::AllDirs);

    QStringList intrsct = PIDs;
    intrsct = list.toSet().intersect(intrsct.toSet()).toList();

    qDebug() << intrsct;
}


void MainWindow::on_actionOpen_triggered()
{
    //QModelIndex index = ui->tableView->currentIndex();
    QModelIndex index = ui->tableView->model()->index(ui->tableView->currentIndex().row(),2);

    if (index.column() == 2)
    {
        QString filePath = ui->tableView->model()->data(index).toString();
        qDebug(filePath.toStdString().c_str());
        //Here we make a pipe
        QString pipePath = filePath + ".pipe";
        QProcess *makepipe = new QProcess;
        makepipe->start("mkfifo", QStringList() << pipePath);

        //pipeReader *PipeReader = new pipeReader;
        //pipeReader(filePath);
/*        QThread *thread = new QThread;
        moveToThread(thread);
        connect(thread, SIGNAL(started()), this, SLOT(pipeReader(QString)));
        thread->start();*/
        QFuture<void> future = QtConcurrent::run(this, &MainWindow::pipeReader, filePath);


        QProcess *process = new QProcess;
        process->start("libreoffice", QStringList() << filePath);

        PIDs.append(QString::number(process->pid()));

        //qDebug() << QObject::trUtf8("processId: ") << QString::number(process->pid()).toStdString().c_str();

        update();
    }
}

void MainWindow::on_actionRefresh_triggered()
{
    connect_db();
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    e->setAccepted( !e->spontaneous());
    if (QMessageBox::Yes == QMessageBox::question(this, "Close Confirmation?",
                                  "Are you sure you want to exit?",
                                  QMessageBox::Yes|QMessageBox::No))
            {
            //system("fly-wmfunc FLYWM_ENABLE_TASKBAR");
            //system("fly-wmfunc FLYWM_UPDATE_VAL TaskbarHeight 40");
            e->accept();
            }
    else
    {
        e->ignore();
    }
}

void MainWindow::on_actionFile_information_triggered()
{
    Properties properties(this);
    properties.exec();
}


Properties::Properties(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Properties"));

    QVBoxLayout* layout = new QVBoxLayout(this);
    QFormLayout* formLayout = new QFormLayout();
    formLayout->setMargin(5);

    QTableView* view;
    MainWindow* ui(static_cast<MainWindow*>(parent));

    QWidget* focus(ui->focusWidget());
    view =  qobject_cast<QTableView *>(focus);

    QModelIndex index = ui->ui->tableView->model()->index(view->currentIndex().row(),2);
    if (index.column() == 2)
    {
        QFileInfo fileInfo(ui->ui->tableView->model()->data(index).toString());

        QLabel* nLabel = new QLabel(fileInfo.fileName());
        formLayout->addRow(tr("Name:"), nLabel);
        QLabel* pLabel = new QLabel(fileInfo.path());
        formLayout->addRow(tr("Path:"), pLabel);
        QLabel* sLabel = new QLabel(QString::number(fileInfo.size()) + " byte");
        formLayout->addRow(tr("Size:"), sLabel);
        QLabel* cLabel = new QLabel(fileInfo.created().toString());
        formLayout->addRow(tr("Created:"), cLabel);
        QLabel* mLabel = new QLabel(fileInfo.lastModified().toString());
        formLayout->addRow(tr("Last Modified:"), mLabel);
        QLabel* rLabel = new QLabel(fileInfo.lastRead().toString());
        formLayout->addRow(tr("Last Read:"), rLabel);
        QLabel* oLabel = new QLabel(fileInfo.owner());
        formLayout->addRow(tr("Owner:"), oLabel);
        QLabel* gLabel = new QLabel(fileInfo.group());
        formLayout->addRow(tr("Group:"), gLabel);

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
        connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        layout->addLayout(formLayout);
        layout->addWidget(buttonBox);
        setLayout(layout);
    }

}

void Properties::accept()
{
    done(1);
}

// As yet unused
void Properties::reject()
{
    done(0);
}
