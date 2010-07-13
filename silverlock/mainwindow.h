#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>

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

private:
    Ui::MainWindow *ui;
    SilverlockPreferences *m_preferences;
    Database *m_database;
    QString m_currentFile;
    bool m_isUntitled;

    QLabel *m_nodeCountStatusLabel;

    void updateNodeActions();
    void populateInfoView(Entry *const entry);
    bool maybeSave();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    MainWindow* findMainWindow(const QString &fileName);

private slots:
    void on_action_New_triggered();
    void on_action_Open_triggered();
    void on_actionClose_triggered();
    bool save();
    bool saveAs();
    void on_action_Print_triggered();
    void on_actionPrint_Preview_triggered();
    void on_actionE_xit_triggered();
    void on_actionMove_Entries_triggered();
    void on_actionEdit_Entry_triggered();
    void on_actionDelete_Entries_triggered();
    void on_actionAdd_Subgroup_triggered();
    void on_actionAdd_Entry_triggered();
    void on_actionMove_Groups_triggered();
    void on_actionEdit_Group_triggered();
    void on_actionDelete_Groups_triggered();
    void on_actionAlways_on_Top_triggered(bool checked);
    void on_actionCenter_to_Screen_triggered();
    void on_actionShow_Passwords_triggered(bool checked);
    void on_actionCheck_for_Updates_triggered();
    void on_actionOptions_triggered();
    void on_actionAbout_triggered();
    void on_infoView_anchorClicked(QUrl url);
    void on_groupBrowser_customContextMenuRequested(QPoint pos);
    void on_entryTable_customContextMenuRequested(QPoint pos);
    void on_groupBrowser_itemSelectionChanged();
    void on_entryTable_itemSelectionChanged();
    void databaseWasModified();
    void clearViews();
};

#endif // MAINWINDOW_H
