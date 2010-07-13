#include "itemnode.h"
#include "database.h"
#include "group.h"
#include "groupnode.h"

ItemNode::ItemNode()
    : DatabaseNode()
{
}

/*!
    Returns the root node in the database tree, searching up the tree recursively.

    If this item has no parent, the method returns NULL.
 */
GroupNode* ItemNode::rootNode() const
{
    if (!this->isAttached())
    {
        return NULL;
    }

    GroupNode *parent = this->m_parent;
    while (parent->parentNode())
    {
        parent = parent->parentNode();
    }

    return parent;
}

void ItemNode::setParentNode(GroupNode *node)
{
    // Can't self-parent
    if (qobject_cast<DatabaseNode*>(this) == qobject_cast<DatabaseNode*>(node))
    {
        qWarning() << "Stopped a self-parent";
        return;
    }

    this->detach();
    this->m_parent = node;
    this->attach();
}

/*!
    Returns whether the item is attached to a parent node.
 */
bool ItemNode::isAttached() const
{
    return this->m_parent;
}

/*!
    Returns a value indicating whether the entry is attached to a database.
 */
bool ItemNode::hasDatabase() const
{
    return qobject_cast<Database*>(this->rootNode());
}

void ItemNode::attach()
{
    if (this->m_parent)
    {
        // Ensure UUID uniquity
        while (this->rootNode()->containsUuid(this->uuid()))
        {
            this->setUuid(QUuid::createUuid());
        }

        // When we add an entry to the parent, the DB was modified
        this->attachToList();

        // Connect our modification signal to the parent's
        // and notify the tree that it has been modified
        QObject::connect(this, SIGNAL(modified()), this->m_parent, SIGNAL(modified()));
        emit this->modified();
    }
}

void ItemNode::detach()
{
    if (this->isAttached())
    {
        this->detachFromList();
        this->m_parent = NULL;

        // Notify the tree that it has been modified and
        // disconnect our modification signal from the parent's
        emit this->modified();
        QObject::disconnect(this, SIGNAL(modified()), this->m_parent, SIGNAL(modified()));
    }
}
