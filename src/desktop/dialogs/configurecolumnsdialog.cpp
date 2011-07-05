#include "configurecolumnsdialog.h"
#include "ui_configurecolumnsdialog.h"
#include "entrytablewidget.h"
#include "entryviewindexes.h"

/*!
    \class ConfigureColumnsDialog

    The ConfigureColumnsDialog class provides a dialog allowing the user to modify the properties
    of columns in the main window entry table.

    Users may use the dialog to show or hide columns, or obscure or reveal their contents in the
    entry table.
 */

/*!
    Constructs a new ConfigureColumnsDialog to modify the columns of \a entryTable.

    The dialog does not take ownership of \a entryTable.

    \param entryTable The EntryTableWidget whose columns can be edited by the dialog.
    \param parent The parent widget of the dialog.
 */
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
    else
    {
        qWarning() << __FUNCTION__ << " given NULL EntryTableWidget";
    }
}

/*!
    Destroys the dialog.
 */
ConfigureColumnsDialog::~ConfigureColumnsDialog()
{
    delete this->ui;
}

/*!
    Shows or hides the corresponding column of, or obscures or reveals the text of the corresponding
    column of, \a item.

    \param item The checkbox item that was clicked.
 */
void ConfigureColumnsDialog::tableItemChanged(QTableWidgetItem *item)
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
