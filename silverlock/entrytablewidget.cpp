#include "entrytablewidget.h"
#include "ui_entrytablewidget.h"
#include "silverlockpreferences.h"
#include <silverlocklib.h>

EntryTableWidget::EntryTableWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EntryTableWidget), m_preferences(NULL)
{
    this->ui->setupUi(this);
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
            uuids.append(QUuid(item->data(6, Qt::UserRole).toString()));
        }
    }

    return uuids;
}

/*!
    Populates the entry table with the the specified group and its subgroups and entries.
 */
void EntryTableWidget::populate(GroupNode *const group)
{
    // Clears the detail view of any previous data.
    this->clear();
    emit this->populating();

    if (this->m_preferences->subgroupsInDetailView())
    {
        // Add all the subgroups of the group
        foreach (Group *subgroup, group->groups())
        {
            QTreeWidgetItem *subgroupItem = new QTreeWidgetItem();
            subgroupItem->setIcon(0, QIcon(":/main/res/group.png"));
            subgroupItem->setText(0, subgroup->title());
            subgroupItem->setText(6, subgroup->uuid());
            this->ui->table->invisibleRootItem()->addChild(subgroupItem);
        }
    }

    // Add all the entries in the group
    foreach (Entry *entry, group->entries())
    {
        QTreeWidgetItem *entryItem = new QTreeWidgetItem();
        entryItem->setIcon(0, QIcon(":/main/res/entry.png"));
        entryItem->setText(0, entry->title());
        entryItem->setText(1, entry->url().toString());
        entryItem->setText(2, entry->username());
        entryItem->setText(3, entry->password());
        entryItem->setText(4, entry->emailAddress());
        entryItem->setText(5, entry->notes());
        entryItem->setText(6, entry->uuid().toString());
        entryItem->setData(6, Qt::UserRole, entry->uuid().toString());
        this->ui->table->invisibleRootItem()->addChild(entryItem);
    }

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
