#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "documentstate.h"
#include "databaseprinterfields.h"

class Database;
class Entry;
class Group;
class SilverlockPreferences;
class InactivityEventFilter;
class ToolbarSearchWidget;
class GroupBrowserWidget;
class EntryTableWidget;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    friend class SilverlockApplication;

public:
    MainWindow(QWidget *parent = NULL);
    ~MainWindow();
    void loadFile(const QString &fileName);
    QString closeFileInternal();
    QByteArray saveEntryTableState() const;
    bool restoreEntryTableState(const QByteArray &state);

    GroupBrowserWidget* groupBrowser() const;
    EntryTableWidget* entryTable() const;

public slots:
    void handleMessage(const QString &message);

signals:
    void needToShow();

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

protected slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    Ui::MainWindow *ui;
    DocumentState m_documentState;
    QLabel *m_nodeCountStatusLabel;

    // References to actions and separator
    QList<QAction*> recentFileActions;
    QAction *separatorAction;
    QTimer *m_clearClipboardTimer;
    QTimer *m_idleTimer;

    ToolbarSearchWidget *m_toolbarSearch;

    qint32 m_lockIdleTimerValue;
    qint32 m_exitIdleTimerValue;

    QAction *widgetsAndToolbarsAction;

    void setupSignals();
    void setupUiAdditional();
    void setupKeyboardShortcuts();
    void setupMenuIcons();

    inline void updateSingleInstance() { this->updateSingleInstance(this); }
    void updateSingleInstance(MainWindow *mw);
    void setNodeCount(Group *group);
    void populateWithSearchResults(const QList<Entry*> &entries, const QString &keywords);
    void populateEntryTable(Group *const group);
    void populateInfoView(Entry *const entry);
    bool maybeSave();
    bool saveFile(const QString &fileName, bool encrypt = true);
    void setCurrentFile(const QString &fileName);
    void clearCurrentFile();
    void updateRecentFileActionsAll();
    void updateRecentFileActions();
    QString strippedName(const QString &fullFileName);

private slots:
    void showPrintDialog(const DatabasePrinterFields &fields = DatabasePrinterFields());
    void findInGroup();
    void newFile();
    void openFile();
    void closeFile();
    bool saveFile();
    bool saveFileAs();
    void changeDatabasePassword();
    void print();
    void printPreview();
    void openRecentFile();
    void lockWorkspace();
    void unlockWorkspace();
    void lockWorkspace(bool lock);
    void toolbarSearch(QString);
    void on_actionExit_triggered();
    void copyEntryFieldValue();
    void clearClipboard();
    void openEntryUrl();
    void openEntryUrlIE();
    void moveEntries();
    void duplicateEntries();
    void editEntries();
    void deleteEntries();
    void addGroupSubgroup();
    void addGroupEntry();
    void moveGroups();
    void editGroups();
    void deleteGroups();
    void find();
    void on_actionAlwaysOnTop_triggered(bool checked);
    void on_actionCenterToScreen_triggered();
    void on_actionConfigureColumns_triggered();
    void on_actionFullScreen_triggered(bool checked);
    void on_actionPreferences_triggered();
    void on_actionHelpContents_triggered();
    void on_actionWebsite_triggered();
    void on_actionDonate_triggered();
    void on_actionCheckForUpdates_triggered();
    void on_actionAboutSilverlock_triggered();
    void on_infoView_anchorClicked(QUrl url);
    void on_groupBrowser_customContextMenuRequested(QPoint pos);
    void on_entryTable_customContextMenuRequested(QPoint pos);
    void on_groupBrowser_itemSelectionChanged();
    void on_entryTable_itemSelectionChanged();
    void updateMenus();
    void updateInterfaceState();
    void databaseWasModified();
    void clearViews();
    void on_unlockWorkspacePushButton_clicked();
    void hideUnlockPassword(bool checked);
    void incrementIdleTimer();
    void resetIdleTimer(QObject *object);
};

#endif // MAINWINDOW_H
