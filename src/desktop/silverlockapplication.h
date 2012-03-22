#ifndef SILVERLOCKAPPLICATION_H
#define SILVERLOCKAPPLICATION_H

#include <petroules-utilities.h>

class ApplicationMenu;
class MainWindow;

class SilverlockApplication : public IntegratedApplication
{
    Q_OBJECT

public:
    explicit SilverlockApplication(int &argc, char *argv[]);
    ~SilverlockApplication();
    static bool isDesktopFileManagerDrop(const QMimeData *mimeData, QStringList *files = NULL);
    bool handleReopen(bool hasVisibleWindows);
    QList<MainWindow*> windows() const;
    MainWindow* activeWindow2() const;

public slots:
    // File menu
    void openFile();
    void openFile(QString fileName);
    void addQueuedFiles(QStringList fileNames);
    void openQueuedFiles();
    void closeFile();

    // Help menu
    void showHelpContents();
    void launchProductWebsite();
    void launchDonationsWebsite();

    // General (these go in separate menus in non-Mac platforms)
    void checkForUpdates();
    void checkForUpdatesSilent();
    void preferences();
    void about();

private:
    void openDelayedFilesInternal();
    MainWindow* windowWithOpenFile(const QString &fileName);
    void createDockMenu();
    class Private;
    Private *d;
};

#define qsApp (qobject_cast<SilverlockApplication*>(qApp))

#endif // SILVERLOCKAPPLICATION_H
