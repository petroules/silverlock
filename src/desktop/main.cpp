#include <QtGui>
#include "silverlockapplication.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    SilverlockApplication instance(argc, argv);

    // Send a string comprised of all but the first argument (the application name) to any other
    // instance that may be running, and bail out if it received it (e.g. it is running)
    QStringList args = instance.arguments();
    args.removeFirst();
    QString message = args.join(" ");
    if (instance.sendMessage(message))
    {
        return 0;
    }

    // No other instance was running, we're good to create and show the first window
    MainWindow *mw = new MainWindow();
    mw->show();

    // We'll handle any command line arguments now
    mw->handleMessage(message);

    return instance.exec();
}
