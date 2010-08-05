#ifndef GROUPBROWSERWIDGET_H
#define GROUPBROWSERWIDGET_H

#include <QtGui>

namespace Ui
{
    class GroupBrowserWidget;
}

class Database;
class Group;

class GroupBrowserWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool multiselect READ multiselect WRITE setMultiselect)

public:
    explicit GroupBrowserWidget(QWidget *parent = NULL);
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
};

#endif // GROUPBROWSERWIDGET_H
