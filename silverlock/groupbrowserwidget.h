#ifndef GROUPBROWSERWIDGET_H
#define GROUPBROWSERWIDGET_H

#include <QtGui>

class Database;
class Group;
class Group;
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
    bool multiselect() const;
    void setMultiselect(bool on);
    QUuid selectedUuid() const;
    QList<QUuid> selectedUuids() const;
    void populate(Database *const database);

public slots:
    void clear();
    void selectAll();

signals:
    void itemSelectionChanged();

    /*!
        Emitted before the group browser is populated with a database's contents.
     */
    void populating();

private:
    void populate(QTreeWidgetItem *item, Group *const group);
    Ui::GroupBrowserWidget *ui;
    SilverlockPreferences *m_preferences;
};

#endif // GROUPBROWSERWIDGET_H
