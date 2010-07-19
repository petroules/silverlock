#include "databasenode.h"
#include "group.h"
#include "database.h"

DatabaseNode::DatabaseNode() :
    QObject(NULL), m_parent(NULL), m_uuid(QUuid::createUuid()), m_title(QString())
{
}

/*!
    This property holds the UUID of the database node.

    UUIDs are used to uniquely identify nodes in a password database. \see Database
 */
QUuid DatabaseNode::uuid() const
{
    return this->m_uuid;
}

/*!
    Sets the UUID of the database node.
 */
void DatabaseNode::setUuid(QUuid uuid)
{
    if (this->m_uuid != uuid)
    {
        emit this->modified();
        this->m_uuid = uuid;
    }
}

QString DatabaseNode::title() const
{
    return this->m_title;
}

void DatabaseNode::setTitle(const QString &title)
{
    if (this->m_title != title)
    {
        emit this->modified();
        this->m_title = title;
    }
}

Group* DatabaseNode::parentNode() const
{
    return this->m_parent;
}

void DatabaseNode::setParentNode(Group *node)
{
    // Can't self-parent or parent to the same parent
    if (this == node || this->m_parent == node)
    {
        return;
    }

    this->detach();
    this->m_parent = node;
    this->attach();
}

/*!
    Returns the root node in the database tree, searching up the tree recursively.

    If this item has no parent, the method returns NULL.
 */
Group* DatabaseNode::rootNode() const
{//TODO
    if (!this->isAttached())
    {
        return NULL;
    }

    Group *parent = this->m_parent;
    while (parent->parentNode())
    {
        parent = parent->parentNode();
    }

    return parent;
}

/*!
    Returns a value indicating whether the node is attached to a database.
 */
bool DatabaseNode::hasDatabase() const
{//TODO
    return qobject_cast<Database*>(this->rootNode());
}

void DatabaseNode::attach()
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

void DatabaseNode::detach()
{
    if (this->isAttached())
    {
        this->detachFromList();
        this->m_parent = NULL;

        // Notify the tree that it has been modified and
        // disconnect our modification signal from the parent's
        emit this->modified();
        QObject::disconnect(this, SIGNAL(modified()), 0, 0);
    }
}
