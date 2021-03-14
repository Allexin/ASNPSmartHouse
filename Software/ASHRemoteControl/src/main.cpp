#include "mainwindow.h"
#include <QApplication>

#include "asnp_utils.h"

int main(int argc, char *argv[])
{
    QApplication::setOrganizationName("ASH");
    QApplication::setApplicationName("ASHRemoteControl");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
