#ifndef ENTRYEDITDIALOG_H
#define ENTRYEDITDIALOG_H

#include "guardeddialog.h"

namespace Ui
{
    class EntryEditDialog;
}

class Entry;

class EntryEditDialog : public GuardedDialog
{
    Q_OBJECT

public:
    EntryEditDialog(Entry *account, QWidget *parent = NULL);
    ~EntryEditDialog();
    Entry* entry() const;
    void setEntry(Entry *account);

protected:
    void load();
    void save();
    void getMessages(QStringList &errors, QStringList &warnings, QStringList &information) const;

private:
    Ui::EntryEditDialog *ui;
    Entry *m_entry;

    void readRecoveryInfo();
    void readCustomFields();
    void readMap(QTableWidget *table, const QMap<QString, QString> &map);
    void writeRecoveryInfo();
    void writeCustomFields();
    bool checkDuplicateRecovery() const;
    bool checkDuplicateCustomFields() const;
    bool checkDuplicateMap(QTableWidget *table) const;
    bool checkRecoveryModified() const;
    bool checkCustomFieldsModified() const;
    bool checkMapModified(QTableWidget *table, const QMap<QString, QString> &map) const;
    void addMapEntry(QTableWidget *table);
    void removeMapEntry(QTableWidget *table);

private slots:
    void hidePassword(bool checked);
    void addRecoveryInfoEntry();
    void addCustomFieldEntry();
    void removeRecoveryInfoEntry();
    void removeCustomFieldEntry();
};

#endif // ENTRYEDITDIALOG_H
