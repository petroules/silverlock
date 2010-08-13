#include <QtGui>
#include <QtSingleApplication>
#include "mainwindow.h"
#include "silverlockpreferences.h"
#include "updatedialog.h"
#include "inactivityeventfilter.h"
#include "version.h"
#include <liel.h>
#ifdef Q_OS_WIN
#include <windows.h>
#endif

void setAppInfo()
{
    ApplicationInfo::setOrganizationName(VER_COMPANYNAME_STR);
    ApplicationInfo::setOrganizationDomain(VER_COMPANYDOMAIN_STR);
    ApplicationInfo::setApplicationName(APP_DISPLAYNAME);
    ApplicationInfo::setUnixName(APP_UNIXNAME);
    ApplicationInfo::setBundleId(APP_BUNDLEID);
    ApplicationInfo::setApplicationVersion(QVersion(VER_PRODUCTVERSION_STR));
    ApplicationInfo::setFileVersion(QVersion(VER_FILEVERSION_STR));
    ApplicationInfo::setCopyright(VER_LEGALCOPYRIGHT_STR);
    ApplicationInfo::setTrademarks(VER_LEGALTRADEMARKS1_STR);

    ApplicationInfo::setUrl(ApplicationInfo::OrganizationHomePage, QUrl("http://www.petroules.com/"));
    ApplicationInfo::setUrl(ApplicationInfo::OrganizationDonations, QUrl("http://www.petroules.com/donate/"));
    ApplicationInfo::setUrl(ApplicationInfo::ApplicationHomePage, QUrl("http://www.petroules.com/products/silverlock/"));
    ApplicationInfo::setUrl(ApplicationInfo::ApplicationHelp, QUrl("http://www.petroules.com/support/silverlock/"));
    ApplicationInfo::setUrl(ApplicationInfo::ApplicationUpdate, QUrl("https://www.petroules.com/version/silverlock/"));
}

int main(int argc, char *argv[])
{
    QtSingleApplication instance("{CA3E50C3-460E-4E12-9EFB-34030DC15306}", argc, argv);
    Q_INIT_RESOURCE(globalresources);
    Q_INIT_RESOURCE(resources);

    // Set application version, copyright and other metadatas
    setAppInfo();
    ApplicationInfo::initialize(instance);

    // Mac OS X: This allows the application to recognize when the user double clicks a file in the Finder
    QObject::connect(&instance, SIGNAL(fileOpenRequest(QString)), &instance, SIGNAL(messageReceived(QString)));

    // Event filter for our idle timer to detect when the user appears to have gone away...
    InactivityEventFilter filter;
    instance.installEventFilter(&filter);

#ifdef Q_OS_WIN
    // We need this call to allow the original process to bring itself to the foreground
    AllowSetForegroundWindow(ASFW_ANY);
#endif

    // Send a string comprised of all but the first argument (the application name) to any other
    // instance that may be running, and bail out if it received it (e.g. it is running)
    QStringList args = instance.arguments();
    args.removeFirst();
    QString message = args.join(" ");
    if (instance.sendMessage(message))
    {
        return 0;
    }

    MainWindow *mw = new MainWindow(&filter);
    mw->show();

    // The update dialog won't interrupt users' flow since internally it is only shown if there is
    // actually an update to download - if not it'll remain hidden and get destroyed by MainWindow's
    // dtor later...
    if (SilverlockPreferences::instance().updateOnStartup())
    {
        UpdateDialog *dialog = new UpdateDialog(mw);
        dialog->setModal(true);
    }

    // If this is the first instance, we'll handle any command line arguments now
    mw->handleMessage(message);

    int code = instance.exec();

    // Save all preferences and free memory
    SilverlockPreferences::instance().destroy();

    return code;
}
