#ifndef ENTRYTABLEWIDGET_H
#define ENTRYTABLEWIDGET_H

#include <QtGui>

class Entry;
class Group;
class SilverlockPreferences;

namespace Ui
{
    class EntryTableWidget;
}

class EntryTableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EntryTableWidget(QWidget *parent = NULL);
    ~EntryTableWidget();
    SilverlockPreferences* preferences() const;
    void setPreferences(SilverlockPreferences *preferences);
    bool columnShown(int index) const;
    void setColumnShown(int index, bool on);
    bool columnObscured(int index) const;
    void setColumnObscured(int index, bool on);
    QUuid selectedUuid() const;
    QList<QUuid> selectedUuids() const;
    void populate(Group *const group);
    void populate(const QList<Entry*> &entries);
    void autoAdjust();

public slots:
    void clear();
    void selectAll();

signals:
    void itemSelectionChanged();

    /*!
        Emitted before the entry table is populated with a group's contents.
     */
    void populating();

private:
    Ui::EntryTableWidget *ui;
    SilverlockPreferences *m_preferences;
    QList<bool> m_isColumnObscured;

    void populateHelper(const QList<Entry*> &entries);
    void setWidgetData(QTreeWidgetItem *item, int index, const QString &data);
};

#endif // ENTRYTABLEWIDGET_H
