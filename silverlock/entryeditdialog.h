#ifndef ENTRYEDITDIALOG_H
#define ENTRYEDITDIALOG_H

#include <QtCore>
#include <QtGui>

class Entry;

namespace Ui {
    class EntryEditDialog;
}

class EntryEditDialog : public QDialog {
    Q_OBJECT
public:
    EntryEditDialog(Entry *account, QWidget *parent = 0);
    ~EntryEditDialog();
    Entry* entry() const;
    void setEntry(Entry *account);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::EntryEditDialog *ui;
    Entry *m_entry;

    void read();
    void write() const;
    QString inputErrorString() const;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
};

#endif // ENTRYEDITDIALOG_H
