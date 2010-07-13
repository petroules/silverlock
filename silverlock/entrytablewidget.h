#ifndef ENTRYTABLEWIDGET_H
#define ENTRYTABLEWIDGET_H

#include <QtGui>

class Group;
class GroupNode;
class SilverlockPreferences;

namespace Ui
{
    class EntryTableWidget;
}

class EntryTableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EntryTableWidget(QWidget *parent = 0);
    ~EntryTableWidget();
    SilverlockPreferences* preferences() const;
    void setPreferences(SilverlockPreferences *preferences);
    QUuid selectedUuid() const;
    QList<QUuid> selectedUuids() const;
    void populate(GroupNode *const group);
    void clear();

signals:
    void itemSelectionChanged();

    /*!
        Emitted before the entry table is populated with a group's contents.
     */
    void populating();

private:
    Ui::EntryTableWidget *ui;
    SilverlockPreferences *m_preferences;
};

#endif // ENTRYTABLEWIDGET_H
