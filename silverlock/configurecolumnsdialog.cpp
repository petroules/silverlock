#include "configurecolumnsdialog.h"
#include "ui_configurecolumnsdialog.h"
#include "entryviewindexes.h"
#include "entrytablewidget.h"

ConfigureColumnsDialog::ConfigureColumnsDialog(EntryTableWidget *entryTable, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigureColumnsDialog), m_entryTable(entryTable)
{
    this->ui->setupUi(this);

    if (entryTable)
    {
        for (int i = 0; i < COLUMN_COUNT; i++)
        {
            QTableWidgetItem *item1 = new QTableWidgetItem();
            item1->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            item1->setCheckState(entryTable->columnShown(i) ? Qt::Checked : Qt::Unchecked);

            QTableWidgetItem *item2 = new QTableWidgetItem();
            item2->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            item2->setCheckState(entryTable->columnObscured(i) ? Qt::Checked : Qt::Unchecked);

            this->ui->tableWidget->setItem(i, 0, item1);
            this->ui->tableWidget->setItem(i, 1, item2);
        }
    }
}

ConfigureColumnsDialog::~ConfigureColumnsDialog()
{
    delete this->ui;
}

void ConfigureColumnsDialog::on_tableWidget_itemChanged(QTableWidgetItem* item)
{
    if (!this->m_entryTable)
    {
        return;
    }

    int row = item->row();
    int column = item->column();

    if (column == 0)
    {
        this->m_entryTable->setColumnShown(row, item->checkState() == Qt::Checked);
    }
    else if (column == 1)
    {
        this->m_entryTable->setColumnObscured(row, item->checkState() == Qt::Checked);
    }
}
