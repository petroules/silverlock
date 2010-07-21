#include <QtGui/QApplication>
#include "mainwindow.h"
#include "version.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName(VER_COMPANYNAME_STR);
    a.setOrganizationDomain(VER_COMPANYDOMAIN_STR);
    a.setApplicationName(VER_PRODUCTNAME_STR);
    a.setApplicationVersion(VER_PRODUCTVERSION_STR);

    MainWindow *w = new MainWindow();
    w->show();

    return a.exec();
}
