#include <QtGui>
#include <QtSingleApplication>
#include "mainwindow.h"
#include "silverlockpreferences.h"
#include "updatedialog.h"
#include "inactivityeventfilter.h"
#include "applicationinfo.h"
#ifdef Q_OS_WIN
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
    QtSingleApplication instance("{CA3E50C3-460E-4E12-9EFB-34030DC15306}", argc, argv);
    ApplicationInfo::initialize(instance);

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
