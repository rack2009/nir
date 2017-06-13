#include "mainwindow.h"
#include <QApplication>
#include <fly/flyui.h>
#include <flyicon.h>

#ifndef SOURCE_VERSION
#define SOURCE_VERSION "0.1.42"
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    flyInit(SOURCE_VERSION, QT_TRANSLATE_NOOP("DMS", "Document management system"));
    a.setWindowIcon(QIcon::fromTheme("accessories-text-editor"));
    MainWindow w;
    w.show();
    return a.exec();
}
