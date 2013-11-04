#include "databasenode.h"
#include "database.h"
#include "group.h"
#include <QtXml>

/*!
    \class DatabaseNode

    The DatabaseNode class is the base class of nodes in a tree structure in which groups and
    password entries are stored.

    Each Group or Database node can contain 0 or more entries (see Entry) and 0 or more groups
    (see Group), which can each contain additional groups and entries, and so on. A Database can
    never have a parent node.

    Each database node is assigned a UUID upon construction to ensure that there is a way to
    uniquely reference any node in the database tree. Uniquity is shared between all nodes
    in a database - no node will have the same UUID as another node in the same database,
    and due to the nature of UUIDs it is extremely unlikely that there will be a collision
    even across separate databases, however this cannot be gauranteed.

    \sa Database, Group, Entry
 */

/*!
    Constructs a new DatabaseNode with the specified title.

    The creation, access and modification times of the node are set to the current UTC time upon
    construction.

    \param title The title, or name, of the node.
 */
DatabaseNode::DatabaseNode(const QString &title) :
    QObject(NULL), m_parent(NULL),
    m_created(QDateTime::currentDateTime().toUTC()),
    m_accessed(QDateTime::currentDateTime().toUTC()),
    m_modified(QDateTime::currentDateTime().toUTC()),
    m_uuid(QUuid::createUuid()), m_title(title)
{
}

/*!
    \property DatabaseNode::created

    This property holds the date and time (in UTC) at which the node was created.

    This property is set automatically when the instance of the node is first created.
 */

QDateTime DatabaseNode::created() const
{
    return this->m_created.toUTC();
}

void DatabaseNode::setCreated(const QDateTime &created)
{
    if (this->m_created != created && created.isValid())
    {
        this->m_created = created.toUTC();
    }
}

/*!
    \property DatabaseNode::accessed

    This property holds the date and time (in UTC) at which the node was last accessed.

    It is up to clients to determine what constitutes "access" of a node. Silverlock
    implements this property by notifying the node that it has been accessed whenever
    an entry or group is selected in their respective display widgets.

    To notify a node that it has been accessed, call setAccessed().
 */

QDateTime DatabaseNode::accessed() const
{
    return this->m_accessed.toUTC();
}

void DatabaseNode::setAccessed(const QDateTime &accessed)
{
    if (this->m_accessed != accessed && accessed.isValid())
    {
        this->m_accessed = accessed.toUTC();
    }
}

/*!
    Sets the last access time of the node to the current date and time (UTC).

    Calling this method will NOT emit the \a treeModified() signal.
 */
void DatabaseNode::setAccessed()
{
    this->m_accessed = QDateTime::currentDateTime().toUTC();
}

/*!
    \property DatabaseNode::modified

    This property holds the date and time (in UTC) at which the node was last modified.

    This property is set automatically whenever a node's properties are modified through
    its mutator methods.
 */

QDateTime DatabaseNode::modified() const
{
    return this->m_modified.toUTC();
}

void DatabaseNode::setModified(const QDateTime &modified)
{
    if (this->m_modified != modified && modified.isValid())
    {
        this->m_modified = modified.toUTC();
    }
}

/*!
    Sets the last modified time and emits the \a treeModified() signal.

    This is a convenience method which should be called by mutator methods of derived classes
    to simultaneously update the last-modified date and emit the \a treeModified() signal.
 */
void DatabaseNode::setModified()
{
    this->setModified(QDateTime::currentDateTime().toUTC());
    emit this->treeModified();
}

/*!
    \property DatabaseNode::uuid

    This property holds the UUID of the database node.

    UUIDs are used to uniquely identify nodes in a password database. When a node is attached
    to a parent node its UUID may be changed to maintain UUID uniquity in the database. See
    DatabaseNode for a full explanation of UUID uniquity within databases.

    \sa DatabaseNode
 */

QUuid DatabaseNode::uuid() const
{
    return this->m_uuid;
}

void DatabaseNode::setUuid(QUuid uuid)
{
    if (this->m_uuid != uuid)
    {
        this->m_uuid = uuid;
        this->setModified();
    }
}

/*!
    \property DatabaseNode::title

    This property holds the title, or name, of the database node.

    Titles are used to allow users to identify nodes. There are no restrictions on multiple nodes
    sharing the same title in a single database.
 */

QString DatabaseNode::title() const
{
    return this->m_title;
}

void DatabaseNode::setTitle(const QString &title)
{
    if (this->m_title != title)
    {
        this->m_title = title;
        this->setModified();
    }
}

/*!
    Gets the parent node of this node.

    See DatabaseNode for an explanation of how the database tree structure works.
 */
Group* DatabaseNode::parentNode() const
{
    return this->m_parent;
}

/*!
    Sets the parent group node of this node.

    When a node's parent is set, it is removed from the tree of any other database it may be
    attached to, and reattached to the database which \a node belongs to, if any.

    A node cannot be set as its own parent. Such a request will be ignored and the node will
    remain attached to its current parent, if any.

    \param node The node to set as the parent node of this node.
 */
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

    If this item has no parent, the method returns \c NULL.
 */
Group* DatabaseNode::rootNode() const
{
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
    Returns a value indicating whether the node is attached to a Database node at root of the tree.
 */
bool DatabaseNode::hasDatabase() const
{
    return dynamic_cast<Database*>(this->rootNode());
}

/*!
    Attaches the node to its parent.

    First, the node's UUID is checked for uniquity within the tree of its parent's root node. Then,
    it attaches itself to its parent's list of child nodes (see attachToList()). Lastly, the node's
    \a treeModified() signal is connected to its parent's so that tree modification notifications
    will be propagated up the database tree from this node to the root node. Finally, the
    \a treeModified() signal is emitted so that the database is notified that a node has been added.

    If the node does not have a reference to a parent, nothing happens.

    \note This method is for internal use only.
 */
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
        QObject::connect(this, SIGNAL(treeModified()), this->m_parent, SIGNAL(treeModified()));
        emit this->treeModified();
    }
}

/*!
    Removes the node from its parent's tree.

    First, the node removes itself from its parent's list of child nodes (see detachFromList())
    and sets the reference to its parent to \c NULL. Then, the node emits its \a treeModified()
    signal so that the database is notified that a node has been removed. Finally, the node
    disconnects its \a treeModified() signal from its parent's.

    If the node is not attached to a parent node, nothing happens.

    \note This method is for internal use only.
 */
void DatabaseNode::detach()
{
    if (this->isAttached())
    {
        this->detachFromList();
        DatabaseNode *parentTemp = this->m_parent;
        this->m_parent = NULL;

        // Notify the tree that it has been modified and
        // disconnect our modification signal from the parent's
        emit this->treeModified();
        QObject::disconnect(this, SIGNAL(treeModified()), parentTemp, 0);
    }
}

/*!
    Reads the node's properties from a QDomElement.

    This method should NEVER be called from a parented database node. It is private and designed
    to be called by the DatabaseReader class only. An assertion failure will occur if the node has
    a parent.

    \param element The element to read from.
 */
void DatabaseNode::fromXml(const QDomElement &element)
{
    Q_ASSERT(this->m_parent == NULL);

    this->setCreated(QDateTime::fromString(element.attribute(XML_CREATED), Qt::ISODate).toUTC());
    this->setAccessed(QDateTime::fromString(element.attribute(XML_ACCESSED), Qt::ISODate).toUTC());
    this->setModified(QDateTime::fromString(element.attribute(XML_MODIFIED), Qt::ISODate).toUTC());
    this->setUuid(element.attribute(XML_UUID));
    this->setTitle(element.attribute(XML_TITLE));
}

/*!
    Creates a QDomElement representing the data within the node, suitable for serialization.

    \param document The QDomDocument to which the generated QDomElement will belong.
 */
QDomElement DatabaseNode::toXml(QDomDocument &document) const
{
    QDomElement element = document.createElement("DatabaseNode");
    element.setAttribute(XML_CREATED, this->created().toUTC().toString(Qt::ISODate) + "Z");
    element.setAttribute(XML_ACCESSED, this->accessed().toUTC().toString(Qt::ISODate) + "Z");
    element.setAttribute(XML_MODIFIED, this->modified().toUTC().toString(Qt::ISODate) + "Z");
    element.setAttribute(XML_UUID, this->uuid().toString());
    element.setAttribute(XML_TITLE, this->title());
    return element;
}
