#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QtGui>
#include <database.h>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    Database *db;
    QString currentFile;
    bool isUntitled;

    void test();
    void populateTreeView(Database *const db);
    void populateTreeView(QTreeWidgetItem *parentItem, Group *const cat);
    void populateDetailView(Group *const cat);
    QUuid selectedGroupUuid() const;
    void documentWasModified();
    bool maybeSave();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    MainWindow* findMainWindow(const QString &fileName);

private slots:
    void on_actionMove_Entries_triggered();
    bool save();
    bool saveAs();
    void on_actionOptions_triggered();
    void on_actionCheck_for_Updates_triggered();
    void on_actionShow_Passwords_triggered(bool checked);
    void on_actionAlways_on_Top_triggered(bool checked);
    void on_actionE_xit_triggered();
    void on_actionPrint_Preview_triggered();
    void on_action_Print_triggered();
    void on_actionClose_triggered();
    void on_action_Open_triggered();
    void on_action_New_triggered();
    void on_actionCenter_to_Screen_triggered();
    void on_actionAbout_triggered();
    void on_treeBrowser_itemSelectionChanged();
    void on_treeBrowser_customContextMenuRequested(QPoint pos);
};

#endif // MAINWINDOW_H
