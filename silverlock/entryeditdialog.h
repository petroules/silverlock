#ifndef ENTRYEDITDIALOG_H
#define ENTRYEDITDIALOG_H

#include <QtGui>

class Entry;

namespace Ui
{
    class EntryEditDialog;
}

class EntryEditDialog : public QDialog
{
    Q_OBJECT

public:
    EntryEditDialog(Entry *account, QWidget *parent = 0);
    ~EntryEditDialog();
    Entry* entry() const;
    void setEntry(Entry *account);

public slots:
    void accept();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::EntryEditDialog *ui;
    Entry *m_entry;

    void read();
    void write() const;
    QString inputErrorString() const;
    bool checkRecoveryModified() const;

private slots:
    void hidePassword(bool checked);
};

#endif // ENTRYEDITDIALOG_H
