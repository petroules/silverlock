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
    bool columnShown(int index) const;
    void setColumnShown(int index, bool on);
    bool columnObscured(int index) const;
    void setColumnObscured(int index, bool on);
    QUuid selectedUuid() const;
    QList<QUuid> selectedUuids() const;
    QString selectedFieldText() const;
    void populate(Group *const group);
    void populate(const QList<Entry*> &entries);
    void autoAdjust();
    QByteArray saveState() const;
    bool restoreState(const QByteArray &state);

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
    QList<bool> m_isColumnObscured;

    void populateHelper(const QList<Entry*> &entries);
    void setWidgetData(QTreeWidgetItem *item, int index, const QString &data);

private slots:
    void on_table_itemDoubleClicked(QTreeWidgetItem* item, int column);
};

#endif // ENTRYTABLEWIDGET_H
