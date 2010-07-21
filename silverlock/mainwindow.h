#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "documentstate.h"

class Database;
class Entry;
class Group;
class SilverlockPreferences;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);

protected slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    Ui::MainWindow *ui;
    SilverlockPreferences *m_preferences;
    DocumentState m_documentState;
    QLabel *m_nodeCountStatusLabel;

    void setupSignals();
    void setupUiAdditional();
    void setupKeyboardShortcuts();
    void setupMenuIcons();

    void setNodeCount(Group *group);
    void populateWithSearchResults(const QList<Entry*> &entries, const QString &keywords);
    void populateEntryTable(Group *const group);
    void populateInfoView(Entry *const entry);
    bool maybeSave();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void closeFile();
    void setCurrentFile(const QString &fileName);
    void clearCurrentFile();
    QString strippedName(const QString &fullFileName);
    MainWindow* findMainWindow(const QString &fileName);

private slots:
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionClose_triggered();
    bool save();
    bool saveAs();
    void on_actionPrint_triggered();
    void on_actionPrintPreview_triggered();
    void on_actionExit_triggered();
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
    void updateInterfaceState();
    void databaseWasModified();
    void clearViews();
};

#endif // MAINWINDOW_H
