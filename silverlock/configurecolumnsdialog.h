#ifndef CONFIGURECOLUMNSDIALOG_H
#define CONFIGURECOLUMNSDIALOG_H

#include <QtGui>

class EntryTableWidget;

namespace Ui
{
    class ConfigureColumnsDialog;
}

class ConfigureColumnsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigureColumnsDialog(EntryTableWidget *entryTable, QWidget *parent = 0);
    ~ConfigureColumnsDialog();

private:
    Ui::ConfigureColumnsDialog *ui;
    EntryTableWidget *m_entryTable;

private slots:
    void on_tableWidget_itemChanged(QTableWidgetItem* item);
};

#endif // CONFIGURECOLUMNSDIALOG_H
