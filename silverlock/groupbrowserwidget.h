#ifndef GROUPBROWSERWIDGET_H
#define GROUPBROWSERWIDGET_H

#include <QtGui>

class Database;
class Group;
class GroupNode;
class SilverlockPreferences;

namespace Ui
{
    class GroupBrowserWidget;
}

class GroupBrowserWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GroupBrowserWidget(QWidget *parent = 0);
    ~GroupBrowserWidget();
    SilverlockPreferences* preferences() const;
    void setPreferences(SilverlockPreferences *preferences);
    QUuid selectedUuid() const;
    QList<QUuid> selectedUuids() const;
    void populate(Database *const database);
    void clear();

signals:
    void itemSelectionChanged();

    /*!
        Emitted before the group browser is populated with a database's contents.
     */
    void populating();

private:
    void populate(QTreeWidgetItem *item, GroupNode *const group);
    Ui::GroupBrowserWidget *ui;
    SilverlockPreferences *m_preferences;
};

#endif // GROUPBROWSERWIDGET_H
