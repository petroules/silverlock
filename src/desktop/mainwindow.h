#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "documentstate.h"

class Database;
class Entry;
class Group;
class SilverlockPreferences;
class InactivityEventFilter;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(InactivityEventFilter *filter, QWidget *parent = NULL);
    ~MainWindow();
    void loadFile(const QString &fileName);
    QByteArray saveEntryTableState() const;
    bool restoreEntryTableState(const QByteArray &state);

public slots:
    void handleMessage(const QString &message);

signals:
    void needToShow();

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);

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

    QLineEdit *m_searchBox;
    QPushButton *m_searchButton;

    qint32 m_lockIdleTimerValue;
    qint32 m_exitIdleTimerValue;

    InactivityEventFilter *m_filter;

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
    void loadFileInWindow(const QString &fileName);
    bool saveFile(const QString &fileName, bool encrypt = true);
    QString closeFile();
    void setCurrentFile(const QString &fileName);
    void clearCurrentFile();
    void updateRecentFileActionsAll();
    void updateRecentFileActions();
    QString strippedName(const QString &fullFileName);
    MainWindow* findMainWindow(const QString &fileName);

private slots:
    void on_actionFindInGroup_triggered();
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionClose_triggered();
    bool save();
    bool saveAs();
    void on_actionChangeDatabasePassword_triggered();
    void on_actionPrint_triggered();
    void on_actionPrintPreview_triggered();
    void openRecentFile();
    void lockWorkspace();
    void unlockWorkspace();
    void lockWorkspace(bool lock);
    void toolbarSearch();
    void on_actionExit_triggered();
    void on_actionCopyFieldValue_triggered();
    void clearClipboard();
    void on_actionDefaultBrowser_triggered();
    void on_actionInternetExplorer_triggered();
    void on_actionMoveEntries_triggered();
    void on_actionDuplicateEntries_triggered();
    void on_actionEditEntry_triggered();
    void on_actionDeleteEntries_triggered();
    void on_actionAddSubgroup_triggered();
    void on_actionAddEntry_triggered();
    void on_actionMoveGroup_triggered();
    void on_actionEditGroup_triggered();
    void on_actionDeleteGroups_triggered();
    void on_actionFind_triggered();
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
    bool isDatabaseSelected();
    void updateInterfaceState();
    void databaseWasModified();
    void clearViews();
    void on_unlockWorkspacePushButton_clicked();
    void hideUnlockPassword(bool checked);
    void incrementIdleTimer();
    void resetIdleTimer(QObject *object);
};

#endif // MAINWINDOW_H
