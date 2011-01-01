#ifndef CONFIGURECOLUMNSDIALOG_H
#define CONFIGURECOLUMNSDIALOG_H

#include <QtGui>

namespace Ui
{
    class ConfigureColumnsDialog;
}

class EntryTableWidget;

class ConfigureColumnsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigureColumnsDialog(EntryTableWidget *entryTable, QWidget *parent = NULL);
    ~ConfigureColumnsDialog();

private:
    Ui::ConfigureColumnsDialog *ui;
    EntryTableWidget *m_entryTable;

private slots:
    void tableItemChanged(QTableWidgetItem *item);
};

#endif // CONFIGURECOLUMNSDIALOG_H
