#include "groupbrowserwidget.h"
#include "ui_groupbrowserwidget.h"
#include "silverlockpreferences.h"
#include <silverlocklib.h>

/*!
    \class GroupBrowserWidget

    The GroupBrowserWidget class provides a widget allowing the user to view and manipulate a tree
    of groups.

    \sa Group
 */

/*!
    Constructs a new GroupBrowserWidget.
 */
GroupBrowserWidget::GroupBrowserWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GroupBrowserWidget), mDatabase(NULL)
{
    this->ui->setupUi(this);
    this->ui->treeBrowser->sortByColumn(0, Qt::AscendingOrder);

    QObject::connect(this->ui->treeBrowser, SIGNAL(itemSelectionChanged()), SIGNAL(itemSelectionChanged()));
    QObject::connect(this->ui->treeBrowser, SIGNAL(customContextMenuRequested(QPoint)), SIGNAL(customContextMenuRequested(QPoint)));
}

/*!
    Destroys the widget.
 */
GroupBrowserWidget::~GroupBrowserWidget()
{
    delete this->ui;
}

Database* GroupBrowserWidget::database() const
{
    return this->mDatabase;
}

void GroupBrowserWidget::setDatabase(Database *database)
{
    this->mDatabase = database;
}

/*!
    \property GroupBrowserWidget::multiselect

    This property holds a value indicating whether the user is allowed to select multiple groups
    simultaneously.
 */

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
    Gets the UUID of the group selected in the tree browser.

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
    Gets a list of the UUIDs of the groups selected in the tree browser.
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
    Determines whether the database node is in the current selection (not necessarily the only selection).
 */
bool GroupBrowserWidget::isDatabaseSelected() const
{
    // No database loaded? Then nothing is selected
    if (!this->mDatabase)
    {
        return false;
    }

    // For each selection, check if it's a database and return true if so
    foreach (QUuid uuid, this->selectedUuids())
    {
        if (dynamic_cast<Database*>(this->mDatabase->findGroup(uuid, true)))
        {
            return true;
        }
    }

    return false;
}

/*!
    Populates the tree view with the specified account database.
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
    this->ui->treeBrowser->expandAll();
}

/*!
    Populates the tree view item with the specified group and its children.
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

/*!
    Clears all data from the tree view.
 */
void GroupBrowserWidget::clear()
{
    this->ui->treeBrowser->clear();
}

/*!
    Selects all groups in the tree view.
 */
void GroupBrowserWidget::selectAll()
{
    this->ui->treeBrowser->selectAll();
}
