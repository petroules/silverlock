#include "silverlockapplication.h"
#include "applicationmenu.h"
#include "mainwindow.h"
#include "silverlockpreferences.h"
#include "version.h"
#include "dialogs/aboutdialog.h"
#include "dialogs/preferencesdialog.h"
#include "dialogs/updatedialog.h"

class SilverlockApplication::Private
{
public:
    Private() : updateDialog(NULL), applicationMenu(NULL), dockMenu(NULL) { }
    ApplicationMenu *applicationMenu;
    UpdateDialog *updateDialog;
    QMenu *dockMenu;
    QStringList openFileQueue;
};

SilverlockApplication::SilverlockApplication(int &argc, char *argv[])
    : IntegratedApplication(VER_APP_UUID, argc, argv), d(new Private())
{
    Q_INIT_RESOURCE(globalresources);
    Q_INIT_RESOURCE(resources);

    this->setOrganizationName(VER_COMPANYNAME_STR);
    this->setOrganizationDomain(VER_COMPANYDOMAIN_STR);
    this->setApplicationName(APP_DISPLAYNAME);
    this->setUnixName(APP_UNIXNAME);
    this->setBundleId(APP_BUNDLEID);
    this->setApplicationVersion(QVersion(VER_PRODUCTVERSION_STR));
    this->setFileVersion(QVersion(VER_FILEVERSION_STR));
    this->setCopyright(VER_LEGALCOPYRIGHT_STR);
    this->setTrademarks(VER_LEGALTRADEMARKS1_STR);

    this->setUrl(OrganizationHomePage, QUrl("http://www.petroules.com/"));
    this->setUrl(OrganizationDonations, QUrl("http://www.petroules.com/donate"));
    this->setUrl(ApplicationHomePage, QUrl("http://www.petroules.com/products/silverlock"));
    this->setUrl(ApplicationHelp, QUrl("http://www.petroules.com/support/silverlock"));
    this->setUrl(ApplicationUpdate, QUrl("https://www.petroules.com/version/silverlock"));

    this->createDockMenu();

    // Create the update dialog and have it do a check within 5 seconds if this was selected in the options
    this->d->updateDialog = new UpdateDialog(NULL);
    this->d->updateDialog->setWindowModality(Qt::ApplicationModal);
    if (UpdateDialog::automaticUpdatesSupported() && SilverlockPreferences::instance().updateOnStartup())
    {
        QTimer::singleShot(5000, this, SLOT(checkForUpdatesSilent()));
    }
}

SilverlockApplication::~SilverlockApplication()
{
    if (this->d)
    {
        if (this->d->updateDialog)
        {
            delete this->d->updateDialog;
        }

        delete this->d;
    }

    // Save all preferences and free memory
    SilverlockPreferences::instance().destroy();
}

bool SilverlockApplication::isDesktopFileManagerDrop(const QMimeData *mimeData, QStringList *files)
{
    if (files)
    {
        files->clear();
    }

    // Extract dropped files from MIME data
    if (!mimeData->hasUrls())
    {
        return false;
    }

    const QList<QUrl> urls = mimeData->urls();
    if (urls.isEmpty())
    {
        return false;
    }

    // Try to find local files
    bool hasFiles = false;
    foreach (const QUrl &url, urls)
    {
        const QString fileName = url.toLocalFile();
        if (!fileName.isEmpty())
        {
            hasFiles = true;

            if (files)
            {
                files->append(fileName);
            }
            else
            {
                // No result list, sufficient for checking
                break;
            }
        }
    }

    return hasFiles;
}

bool SilverlockApplication::dockIconClicked(bool hasVisibleWindows)
{
    if (!hasVisibleWindows)
    {
        MainWindow *window = new MainWindow();
        window->show();
    }

    return true;
}

QList<MainWindow*> SilverlockApplication::windows() const
{
    QList<MainWindow*> list;

    foreach (QWidget *widget, this->topLevelWidgets())
    {
        MainWindow *window = qobject_cast<MainWindow*>(widget);
        if (window)
        {
            list.append(window);
        }
    }

    return list;
}


MainWindow* SilverlockApplication::activeWindow2() const
{
    return qobject_cast<MainWindow*>(this->activeWindow());
}

void SilverlockApplication::createDockMenu()
{
    this->d->dockMenu = new QMenu();
    this->d->dockMenu->addAction(tr("Check for Updates"), this, SLOT(checkForUpdates()));
    this->d->dockMenu->addSeparator();
    this->d->dockMenu->addAction(tr("Preferences..."), this, SLOT(preferences()), QKeySequence::Preferences);
    this->macSetDockMenu(this->d->dockMenu);
}

void SilverlockApplication::openFile()
{
    NativeOpenFileDialog dialog(this->activeWindow2(), false, tr("Open"), QString(), tr("Silverlock databases (*.sdbx)"));
    if (dialog.exec() == QDialog::Accepted)
    {
        this->openFile(dialog.selectedFile());
    }
}

void SilverlockApplication::openFile(QString fileName)
{
    // Blank files do nothing
    if (fileName.isEmpty())
    {
        return;
    }

    // If this file is already open in another window, show that window
    MainWindow *existing = this->windowWithOpenFile(fileName);
    if (existing)
    {
        existing->show();
        existing->raise();
        existing->activateWindow();
        return;
    }

    // If this window does not have a file loaded and hasn't been modified, open the file in this window
    if (this->activeWindow2() && !this->activeWindow2()->m_documentState.hasDocument())
    {
        this->activeWindow2()->loadFile(fileName);
    }
    else
    {
        // If there's some existing windows, see if any of them can take it
        foreach (MainWindow *window, this->windows())
        {
            if (!window->m_documentState.hasDocument())
            {
                window->loadFile(fileName);
                return;
            }
        }

        // Otherwise create a new window to open it
        MainWindow *other = new MainWindow();
        other->move(this->activeWindow2() ? (this->activeWindow2()->pos() + QPoint(40, 40)) : QPoint());
        other->show();
        other->loadFile(fileName);

        // If it didn't load successfully, bail out
        if (!other->m_documentState.hasDocument())
        {
            delete other;
            return;
        }
    }
}

void SilverlockApplication::addQueuedFiles(QStringList fileNames)
{
    this->d->openFileQueue.append(fileNames);
}

void SilverlockApplication::openQueuedFiles()
{
    while (!this->d->openFileQueue.isEmpty())
    {
        this->openFile(this->d->openFileQueue.first());
        this->d->openFileQueue.removeFirst();
    }
}

void SilverlockApplication::closeFile()
{
    MainWindow *window = this->activeWindow2();
    if (window)
    {
        window->closeFileInternal();
    }
}

void SilverlockApplication::showHelpContents()
{
    QDesktopServices::openUrl(this->url(ApplicationHelp));
}

void SilverlockApplication::launchProductWebsite()
{
    QDesktopServices::openUrl(this->url(ApplicationHomePage));
}

void SilverlockApplication::launchDonationsWebsite()
{
    QDesktopServices::openUrl(this->url(OrganizationDonations));
}

/*!
    Displays the check for updates dialog.
 */
void SilverlockApplication::checkForUpdates()
{
    this->d->updateDialog->show();
    this->checkForUpdatesSilent();
}

/*!
    Checks for updates and only shows the dialog if an update is available.
 */
void SilverlockApplication::checkForUpdatesSilent()
{
    this->d->updateDialog->check();
}

void SilverlockApplication::preferences()
{
    PreferencesDialog preferences;
    if (preferences.exec())
    {
        // Update recent file actions in case they were cleared
        //this->updateRecentFileActions(All?)();
    }
}

void SilverlockApplication::about()
{
    AboutDialog dialog;
    dialog.exec();
}

/*!
    Finds the window which has the file with the specified file name open.
    Returns NULL if the file is not opened by any window.

    \param fileName The name of the file to find the open window of.
 */
MainWindow* SilverlockApplication::windowWithOpenFile(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    // This could happen if the file didn't exist - otherwise we could get a false positive!
    if (canonicalFilePath.isEmpty())
    {
        return NULL;
    }

    foreach (MainWindow *window, this->windows())
    {
        if (window && window->m_documentState.currentFile() == canonicalFilePath)
        {
            return window;
        }
    }

    return NULL;
}
