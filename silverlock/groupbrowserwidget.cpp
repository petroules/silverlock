#include "groupbrowserwidget.h"
#include "ui_groupbrowserwidget.h"
#include "silverlockpreferences.h"
#include <silverlocklib.h>

GroupBrowserWidget::GroupBrowserWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GroupBrowserWidget), m_preferences(NULL)
{
    this->ui->setupUi(this);
    QObject::connect(this->ui->treeBrowser, SIGNAL(itemSelectionChanged()), SIGNAL(itemSelectionChanged()));
    QObject::connect(this->ui->treeBrowser, SIGNAL(customContextMenuRequested(QPoint)), SIGNAL(customContextMenuRequested(QPoint)));
}

GroupBrowserWidget::~GroupBrowserWidget()
{
    delete this->ui;
}

SilverlockPreferences* GroupBrowserWidget::preferences() const
{
    return this->m_preferences;
}

void GroupBrowserWidget::setPreferences(SilverlockPreferences *preferences)
{
    this->m_preferences = preferences;
}

bool GroupBrowserWidget::multiselect() const
{
    return this->ui->treeBrowser->selectionMode() == QAbstractItemView::ExtendedSelection;
}

void GroupBrowserWidget::setMultiselect(bool on)
{
    if (on)
    {
        this->ui->treeBrowser->setSelectionMode(QAbstractItemView::ExtendedSelection);
    }
    else
    {
        this->ui->treeBrowser->setSelectionMode(QAbstractItemView::SingleSelection);
    }
}

/*!
    Gets the UUID of the group selected in the left-hand tree browser.

    This method returns empty UUID if no group or more than one group is selected.
 */
QUuid GroupBrowserWidget::selectedUuid() const
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
    Gets a list of the UUIDs of the groups selected in the left-hand tree browser.
 */
QList<QUuid> GroupBrowserWidget::selectedUuids() const
{
    QList<QUuid> uuids;
    QList<QTreeWidgetItem*> selected = this->ui->treeBrowser->selectedItems();
    foreach (QTreeWidgetItem *item, selected)
    {
        if (item)
        {
            uuids.append(QUuid(item->data(0, Qt::UserRole).toString()));
        }
    }

    return uuids;
}

/*!
    Populates the left-hand tree view with the specified account database.
 */
void GroupBrowserWidget::populate(Database *const database)
{
    // Remove any previous items from all views
    this->clear();
    emit this->populating();

    // Make sure database exists
    if (!database)
    {
        return;
    }

    // Get the root item of the tree browser and call the recursive
    // populate function with the account database's root category
    QTreeWidgetItem *parentItem = this->ui->treeBrowser->invisibleRootItem();
    this->populate(parentItem, database);

    // Then expand all the nodes
    if (this->m_preferences->expandTreeViewNodes())
    {
        this->ui->treeBrowser->expandAll();
    }
}

/*!
    Populates the left-hand tree view item with the specified group and its children.
 */
void GroupBrowserWidget::populate(QTreeWidgetItem *parentItem, Group *const group)
{
    // If the cast to Database is not null then the GroupNode is a database
    bool isDatabase = qobject_cast<Database*>(group);

    // Create a widget item for the group, set the text, icon, etc., and add it to the parent item
    QTreeWidgetItem *groupItem = new QTreeWidgetItem();
    groupItem->setText(0, group->title());
    groupItem->setData(0, Qt::UserRole, group->uuid().toString());

    if (isDatabase)
    {
        groupItem->setIcon(0, QIcon(":/main/res/app.svg"));
    }
    else
    {
        groupItem->setIcon(0, this->style()->standardIcon(QStyle::SP_DirIcon));
    }


    // The database node should have bold text!
    if (isDatabase)
    {
        QFont font = groupItem->font(0);
        font.setBold(true);
        groupItem->setFont(0, font);
    }

    // Add it to the parent
    parentItem->addChild(groupItem);

    // Then add all the category's subcategories as children of itself
    foreach (Group *node, group->groups())
    {
        this->populate(groupItem, node);
    }
}

void GroupBrowserWidget::clear()
{
    this->ui->treeBrowser->clear();
}

void GroupBrowserWidget::selectAll()
{
    this->ui->treeBrowser->selectAll();
}
