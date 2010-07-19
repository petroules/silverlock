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
    explicit EntryTableWidget(QWidget *parent = 0);
    ~EntryTableWidget();
    SilverlockPreferences* preferences() const;
    void setPreferences(SilverlockPreferences *preferences);
    bool passwordsShown() const;
    void setPasswordsShown(bool on);
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

    void populateHelper(const QList<Entry*> &entries);
};

#endif // ENTRYTABLEWIDGET_H
