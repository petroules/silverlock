#include <QtGui>
#include <QtSingleApplication>
#include "version.h"
#include "mainwindow.h"
#include "silverlockpreferences.h"
#include "updatedialog.h"
#include "inactivityeventfilter.h"
#ifdef Q_OS_WIN
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
    QtSingleApplication instance(QString("%1 %2 %3")
        .arg(VER_COMPANYNAME_STR).arg(VER_PRODUCTNAME_STR).arg(VER_PRODUCTVERSION_STR), argc, argv);
    Q_INIT_RESOURCE(globalresources);
    Q_INIT_RESOURCE(resources);
    InactivityEventFilter filter;
    instance.installEventFilter(&filter);

#ifdef Q_OS_WIN
    // We need this call to allow the original process to bring itself to the foreground
    AllowSetForegroundWindow(ASFW_ANY);
#endif

    // Send a string comprised of all but the first argument
    // to the other applications and bail out if it received it
    QStringList args = instance.arguments();
    args.removeFirst();
    QString message = args.join(" ");
    if (instance.sendMessage(message))
    {
        return 0;
    }

    // Set the application information
    instance.setOrganizationName(VER_COMPANYNAME_STR);
    instance.setOrganizationDomain(VER_COMPANYDOMAIN_STR);
    instance.setApplicationName(VER_PRODUCTNAME_STR);
    instance.setApplicationVersion(VER_PRODUCTVERSION_STR);

    // Create a new main window
    MainWindow *mw = new MainWindow(&filter);
    mw->show();

    UpdateDialog *dialog = NULL;
    if (SilverlockPreferences::instance().updateOnStartup())
    {
         dialog = new UpdateDialog(mw);
         dialog->setModal(true);
    }

    mw->handleMessage(message);

    int code = instance.exec();

    // Save settings and free memory
    SilverlockPreferences::instance().destroy();

    return code;
}
