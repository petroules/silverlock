#include "entryeditdialog.h"
#include "ui_entryeditdialog.h"
#include <silverlocklib.h>

/*!
    \file entryeditdialog_helper.cpp

    This file contains the helper methods for checking data validity in the EntryEditDialog class,
    to reduce clutter.
 */

/*!
    Reads recovery questions and answers from the Entry instance and populates the recovery info
    table with the data.

    \sa readMap()
 */
void EntryEditDialog::readRecoveryInfo()
{
    this->readMap(this->ui->recoveryTableWidget, this->m_entry->recoveryInfo());
}

/*!
    Reads custom fields from the Entry instance and populates the custom fields table with the data.

    \sa readMap()
 */
void EntryEditDialog::readCustomFields()
{
    this->readMap(this->ui->customFieldsTableWidget, this->m_entry->customFields());
}

/*!
    Reads data from \a map and populates the specified table with the data.

    \param table The table to put the data into.
    \param map The map to read the data from. This map will represent either recovery
    question/answer pairs, or custom fields.
 */
void EntryEditDialog::readMap(QTableWidget *table, const QMap<QString, QString> &map)
{
    // Clear any previous data and set the row count equal to the number of pairs we have
    table->clearContents();
    table->setRowCount(map.count());

    // Go through each pair with a hash iterator...
    QMapIterator<QString, QString> i(map);
    int index = 0;
    while (i.hasNext())
    {
        i.next();

        // Create table widget items for the key and value and set them on the table
        table->setItem(index, 0, new QTableWidgetItem(i.key()));
        table->setItem(index, 1, new QTableWidgetItem(i.value()));

        index++;
    }
}

/*!
    Updates the recovery info in the Entry instance if necessary.
 */
void EntryEditDialog::writeRecoveryInfo()
{
    QTableWidget *const table = this->ui->recoveryTableWidget;

    if (this->checkMapModified(table, this->m_entry->recoveryInfo()))
    {
        this->m_entry->clearRecoveryInfo();
        for (int i = 0; i < table->rowCount(); i++)
        {
            QString key = table->item(i, 0)->text();
            QString value = table->item(i, 1)->text();
            this->m_entry->insertRecoveryInfo(key, value);
        }
    }
}

/*!
    Updates the custom fields in the Entry instance if necessary.
 */
void EntryEditDialog::writeCustomFields()
{
    QTableWidget *const table = this->ui->customFieldsTableWidget;

    if (this->checkMapModified(table, this->m_entry->customFields()))
    {
        this->m_entry->clearCustomFields();
        for (int i = 0; i < table->rowCount(); i++)
        {
            QString key = table->item(i, 0)->text();
            QString value = table->item(i, 1)->text();
            this->m_entry->insertCustomField(key, value);
        }
    }
}

/*!
    Checks if the user has entered multiple recovery questions with the same question text.
 */
bool EntryEditDialog::checkDuplicateRecovery() const
{
    return this->checkDuplicateMap(this->ui->recoveryTableWidget);
}

/*!
    Checks if the user has entered multiple custom fields with the same name.
 */
bool EntryEditDialog::checkDuplicateCustomFields() const
{
    return this->checkDuplicateMap(this->ui->customFieldsTableWidget);
}

/*!
    Checks if any of the cells in the first column of the specified table widget have the same text.

    This is used to check if duplicate keys were entered in the recovery info or custom fields.

    \param table The table to check for duplicates.
 */
bool EntryEditDialog::checkDuplicateMap(QTableWidget *table) const
{
    QStringList list;
    for (int i = 0; i < table->rowCount(); i++)
    {
        list.append(table->item(i, 0)->text());
    }

    // If the number of removed entries is greater than 0, SOMETHING was removed, thus
    // there was at least one duplicate
    return list.removeDuplicates() > 0;
}

/*!
    Returns a value indicating whether the user actually made any changes to the recovery info.

    \sa checkMapModified()
 */
bool EntryEditDialog::checkRecoveryModified() const
{
    return this->checkMapModified(this->ui->recoveryTableWidget, this->m_entry->recoveryInfo());
}

/*!
    Returns a value indicating whether the user actually made any changes to the custom fields.

    \sa checkMapModified()
 */
bool EntryEditDialog::checkCustomFieldsModified() const
{
    return this->checkMapModified(this->ui->customFieldsTableWidget, this->m_entry->customFields());
}

/*!
    Returns a value indicating whether the user actually made any changes to the recovery
    questions/answers or custom fields.

    Because of the way these types of data are stored in an Entry, this lets us
    easily just clear the array in the Entry, and re-add everything from the table view,
    without emitting a modified signal when nothing has actually been modified.
 */
bool EntryEditDialog::checkMapModified(QTableWidget *table, const QMap<QString, QString> &map) const
{
    // Make sure to sort the table so it's in the same order as the entry for comparing
    table->sortItems(0, Qt::AscendingOrder);

    // If the number of items differ we know they're different right away
    if (table->rowCount() != map.count())
    {
        return true;
    }

    // Now let's loop through all the items and check if the key or value
    // for each pair differs from its counterpart in the table... we can do this
    // linearly since we sorted the table earlier
    QMapIterator<QString, QString> i(map);
    int index = 0;
    while (i.hasNext())
    {
        i.next();

        // Get the question and answer from the table...
        QString key = table->item(index, 0)->text();
        QString value = table->item(index, 1)->text();

        // ...and if either of them don't match the entry itself, there was a modification
        if (i.key() != key || i.value() != value)
        {
            return true;
        }

        index++;
    }

    return false;
}

/*!
    Adds a blank row to the end of the recovery info table.
 */
void EntryEditDialog::addRecoveryInfoEntry()
{
    this->addMapEntry(this->ui->recoveryTableWidget);
}

/*!
    Adds a blank row to the end of the custom fields table.
 */
void EntryEditDialog::addCustomFieldEntry()
{
    this->addMapEntry(this->ui->customFieldsTableWidget);
}

/*!
    Adds a blank row to the end of \a table.

    \param table The table to add the row to.
 */
void EntryEditDialog::addMapEntry(QTableWidget *table)
{
    int row = table->rowCount();
    table->insertRow(row);
    table->setItem(row, 0, new QTableWidgetItem());
    table->setItem(row, 1, new QTableWidgetItem());
}

/*!
    Removes the selected row from the recovery info table.
 */
void EntryEditDialog::removeRecoveryInfoEntry()
{
    this->removeMapEntry(this->ui->recoveryTableWidget);
}

/*!
    Removes the selected row from the custom fields table.
 */
void EntryEditDialog::removeCustomFieldEntry()
{
    this->removeMapEntry(this->ui->customFieldsTableWidget);
}

/*!
    Removes the selected row from the \a table.

    \param table The table to remove the selected row from.
 */
void EntryEditDialog::removeMapEntry(QTableWidget *table)
{
    QTableWidgetItem *item = table->currentItem();
    if (item)
    {
        table->removeRow(item->row());
    }
}
