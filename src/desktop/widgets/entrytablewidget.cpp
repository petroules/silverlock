#include "entrytablewidget.h"
#include "ui_entrytablewidget.h"
#include "entryviewindexes.h"
#include "silverlockpreferences.h"
#include <silverlocklib.h>

/*!
    \class EntryTableWidget

    The EntryTableWidget class provides a widget allowing the user to view and manipulate a list of
    entries.

    \sa Entry
 */

/*!
    Constructs a new EntryTableWidget.
 */
EntryTableWidget::EntryTableWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EntryTableWidget)
{
    this->ui->setupUi(this);
    this->setFocusProxy(this->ui->table);
    this->ui->table->sortByColumn(COLUMN_TITLE, Qt::AscendingOrder);
    this->ui->table->setAttribute(Qt::WA_MacShowFocusRect, false);

    // All columns' obscured flags should be false by default
    for (int i = 0; i < COLUMN_COUNT; i++)
    {
        this->m_isColumnObscured.append(false);
    }

    // Obscure the password column
    this->setColumnObscured(COLUMN_PASSWORD, true);
    this->setColumnShown(COLUMN_UUID, false);

    QObject::connect(this->ui->table, SIGNAL(itemSelectionChanged()), SIGNAL(itemSelectionChanged()));
    QObject::connect(this->ui->table, SIGNAL(customContextMenuRequested(QPoint)), SIGNAL(customContextMenuRequested(QPoint)));
}

/*!
    Destroys the widget.
 */
EntryTableWidget::~EntryTableWidget()
{
    delete this->ui;
}

/*!
    Returns a value indicating whether the column specified by the logical index \a index is
    visible.

    \param index The index of the column to check the visibility of.
 */
bool EntryTableWidget::columnShown(int index) const
{
    return !this->ui->table->isColumnHidden(index);
}

/*!
    Shows or hides the column specified by the logical index \a index.

    \param on \c true to show column \a index; \c false to hide it.
 */
void EntryTableWidget::setColumnShown(int index, bool on)
{
    this->ui->table->setColumnHidden(index, !on);
}

/*!
    Returns a value indicating whether the text of the column specified by the logical index
    \a index is obscured.

    \param index The index of the column to check the text visibility of.
 */
bool EntryTableWidget::columnObscured(int index) const
{
    return this->m_isColumnObscured.at(index);
}

/*!
    Obscures or reveals the text of the column specified by the logical index \a index.

    \param on \c true to obscure text in column \a index; \c false to reveal it.
 */
void EntryTableWidget::setColumnObscured(int index, bool on)
{
    this->m_isColumnObscured[index] = on;

    QTreeWidgetItem *root = this->ui->table->invisibleRootItem();
    for (int i = 0; i < root->childCount(); i++)
    {
        QTreeWidgetItem *item = root->child(i);
        item->setText(index, on ? QString(OBSCURETEXT) : item->data(index, Qt::UserRole).toString());
    }
}

/*!
    Gets the UUID of the entry selected in the right-hand table view.

    This method returns empty UUID if no entry or more than one entry is selected.
 */
QUuid EntryTableWidget::selectedUuid() const
{
    // Get the list of selected items and make sure there's only 1 selected
    QList<QUuid> selected = this->selectedUuids();
    if (selected.count() == 1)
    {
        return selected.first();
    }

    return QUuid();
}

/*!
    Gets a list of the UUIDs of the entries selected in the right-hand table view.
 */
QList<QUuid> EntryTableWidget::selectedUuids() const
{
    QList<QUuid> uuids;
    QList<QTreeWidgetItem*> selected = this->ui->table->selectedItems();
    foreach (QTreeWidgetItem *item, selected)
    {
        if (item)
        {
            uuids.append(QUuid(item->data(COLUMN_UUID, DATA_REALTEXT).toString()));
        }
    }

    return uuids;
}

/*!
    Gets the real text in the currently selected field.
 */
QString EntryTableWidget::selectedFieldText() const
{
    QList<QTreeWidgetItem*> items = this->ui->table->selectedItems();
    if (items.count() == 1)
    {
        QTreeWidgetItem *item = items.first();
        if (item)
        {
            return item->data(this->ui->table->currentColumn(), DATA_REALTEXT).toString();
        }
    }

    return QString();
}

/*!
    Populates the entry table with the the specified group's entries.

    \param group The group to populate the table with.
 */
void EntryTableWidget::populate(Group *const group)
{
    // Clear the detail view of any previous data
    this->ui->table->clear();
    emit this->populating();

    if (group)
    {
        // Add all the entries in the group
        this->populateHelper(group->entries());
    }

    this->autoAdjust();
}

/*!
    Populates the entry table with the specified list of entries.

    \param entries The list of entries to populate the table with.
 */
void EntryTableWidget::populate(const QList<Entry*> &entries)
{
    // Clear the detail view of any previous data
    this->ui->table->clear();
    emit this->populating();

    this->populateHelper(entries);
    this->autoAdjust();
}

/*!
    \internal
 */
void EntryTableWidget::populateHelper(const QList<Entry*> &entries)
{
    foreach (Entry *entry, entries)
    {
        QTreeWidgetItem *entryItem = new QTreeWidgetItem();
        entryItem->setIcon(COLUMN_TITLE, this->style()->standardIcon(QStyle::SP_FileIcon));
        this->setWidgetData(entryItem, COLUMN_TITLE, entry->title());
        this->setWidgetData(entryItem, COLUMN_USERNAME, entry->username());
        this->setWidgetData(entryItem, COLUMN_PASSWORD, entry->password());
        this->setWidgetData(entryItem, COLUMN_URL, entry->url().toString());
        this->setWidgetData(entryItem, COLUMN_NOTES, entry->notes());
        this->setWidgetData(entryItem, COLUMN_UUID, entry->uuid().toString());
        this->ui->table->invisibleRootItem()->addChild(entryItem);
    }

    this->ui->stackedWidget->setCurrentWidget(entries.count() > 0 ? this->ui->tablePage : this->ui->messagePage);
    this->ui->label->setText(tr("There are no entries contained within this group or your search returned no results."));
}

/*!
    Sets the data of the widget item \a item.

    \param item The item to set the data of.
    \param index The index of the column whose data is being set.
    \param data The data to set.
 */
void EntryTableWidget::setWidgetData(QTreeWidgetItem *item, int index, const QString &data)
{
    item->setText(index, !this->m_isColumnObscured.at(index) ? data : QString(OBSCURETEXT));
    item->setData(index, DATA_REALTEXT, data);
}

/*!
    Causes all columns to auto-adjust to their contents.
 */
void EntryTableWidget::autoAdjust()
{
    int cols = this->ui->table->header()->count();
    for (int i = 0; i < cols; i++)
    {
        this->ui->table->header()->setResizeMode(i, QHeaderView::ResizeToContents);
    }
}

/*!
    Clears all data from the table.
 */
void EntryTableWidget::clear()
{
    this->ui->table->clear();

    this->ui->stackedWidget->setCurrentWidget(this->ui->messagePage);
    this->ui->label->setText(tr("Select a group on the left to display the entries it contains, or press Ctrl+F to perform a search."));
}

/*!
    Selects all entries in the table.
 */
void EntryTableWidget::selectAll()
{
    this->ui->table->selectAll();
}

/*!
    Performs actions for double-clicking on table cells depending on the column the cell belongs to.

    Currently, this includes:

    \li URL column: opening the URL of the Entry represented by \a item in the system web browser.

    \param item The item that was clicked.
    \param column The column that was clicked.
 */
void EntryTableWidget::on_table_itemDoubleClicked(QTreeWidgetItem* item, int column)
{
    if (column == COLUMN_URL)
    {
        QDesktopServices::openUrl(QUrl(item->data(COLUMN_URL, DATA_REALTEXT).toString()));
    }
}

/*!
    Saves the current state of this header view.

    To restore the saved state, pass the return value to restoreState().

    \sa restoreState()
 */
QByteArray EntryTableWidget::saveState() const
{
    return this->ui->table->header()->saveState();
}

/*!
    Restores the \a state of this header view. This function returns \c true if the state was
    restored; otherwise returns \c false.

    \sa saveState()
 */
bool EntryTableWidget::restoreState(const QByteArray &state)
{
    return this->ui->table->header()->restoreState(state);
}
