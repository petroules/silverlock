#include <QtGui>
#if (defined(Q_OS_ANDROID) || defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_OS_WINCE))
#define Q_IS_MOBILE
#endif

#ifdef Q_IS_MOBILE
#include "mobile/mainwindow.h"
#else
#include "silverlockapplication.h"
#include "mainwindow.h"
#endif

int main(int argc, char *argv[])
{
#ifdef Q_IS_MOBILE
    QApplication instance(argc, argv);

    MainWindow mainWindow;
    mainWindow.setOrientation(MainWindow::ScreenOrientationAuto);
    mainWindow.showExpanded();
#else
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
#endif

    return instance.exec();
}
