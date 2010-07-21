#include "entrytablewidget.h"
#include "ui_entrytablewidget.h"
#include "entryviewindexes.h"
#include "silverlockpreferences.h"
#include <silverlocklib.h>

EntryTableWidget::EntryTableWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EntryTableWidget), m_preferences(NULL)
{
    this->ui->setupUi(this);
    this->ui->table->sortByColumn(COLUMN_TITLE, Qt::AscendingOrder);

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

EntryTableWidget::~EntryTableWidget()
{
    delete this->ui;
}

SilverlockPreferences* EntryTableWidget::preferences() const
{
    return this->m_preferences;
}

void EntryTableWidget::setPreferences(SilverlockPreferences *preferences)
{
    this->m_preferences = preferences;
}

bool EntryTableWidget::columnShown(int index) const
{
    return !this->ui->table->isColumnHidden(index);
}

void EntryTableWidget::setColumnShown(int index, bool on)
{
    this->ui->table->setColumnHidden(index, !on);
}

bool EntryTableWidget::columnObscured(int index) const
{
    return this->m_isColumnObscured.at(index);
}

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
    Populates the entry table with the the specified group and its subgroups and entries.
 */
void EntryTableWidget::populate(Group *const group)
{
    // Clear the detail view of any previous data
    this->clear();
    emit this->populating();

    if (group)
    {
        // Add all the entries in the group
        this->populateHelper(group->entries());
    }

    this->autoAdjust();
}

void EntryTableWidget::populate(const QList<Entry*> &entries)
{
    // Clear the detail view of any previous data
    this->clear();
    emit this->populating();
    this->populateHelper(entries);
    this->autoAdjust();
}

void EntryTableWidget::populateHelper(const QList<Entry*> &entries)
{
    foreach (Entry *entry, entries)
    {
        QTreeWidgetItem *entryItem = new QTreeWidgetItem();
        entryItem->setIcon(COLUMN_TITLE, this->style()->standardIcon(QStyle::SP_FileIcon));
        this->setWidgetData(entryItem, COLUMN_TITLE, entry->title());
        this->setWidgetData(entryItem, COLUMN_URL, entry->url().toString());
        this->setWidgetData(entryItem, COLUMN_USERNAME, entry->username());
        this->setWidgetData(entryItem, COLUMN_PASSWORD, entry->password());
        this->setWidgetData(entryItem, COLUMN_EMAIL, entry->emailAddress());
        this->setWidgetData(entryItem, COLUMN_NOTES, entry->notes());
        this->setWidgetData(entryItem, COLUMN_UUID, entry->uuid().toString());
        this->ui->table->invisibleRootItem()->addChild(entryItem);
    }
}

void EntryTableWidget::setWidgetData(QTreeWidgetItem *item, int index, const QString &data)
{
    item->setText(index, !this->m_isColumnObscured.at(index) ? data : QString(OBSCURETEXT));
    item->setData(index, DATA_REALTEXT, data);
}

void EntryTableWidget::autoAdjust()
{
    // Tell the columns to auto-adjust to their contents
    int cols = this->ui->table->header()->count();
    for (int i = 0; i < cols; i++)
    {
        this->ui->table->header()->setResizeMode(i, QHeaderView::ResizeToContents);
    }
}

void EntryTableWidget::clear()
{
    this->ui->table->clear();
}

void EntryTableWidget::selectAll()
{
    this->ui->table->selectAll();
}
