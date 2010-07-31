#include "databasenode.h"
#include "group.h"
#include "database.h"

DatabaseNode::DatabaseNode(const QString &title) :
    QObject(NULL), m_parent(NULL),
    m_created(QDateTime::currentDateTime().toUTC()),
    m_accessed(QDateTime::currentDateTime().toUTC()),
    m_modified(QDateTime::currentDateTime().toUTC()),
    m_uuid(QUuid::createUuid()), m_title(title)
{
}

QDateTime DatabaseNode::created() const
{
    return this->m_created;
}

void DatabaseNode::setCreated(const QDateTime &created)
{
    if (this->m_created != created && created.isValid())
    {
        this->m_created = created.toUTC();
    }
}

QDateTime DatabaseNode::accessed() const
{
    return this->m_accessed;
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
    This will NOT emit the \a modified() signal.
 */
void DatabaseNode::setAccessed()
{
    this->m_accessed = QDateTime::currentDateTime().toUTC();
}

QDateTime DatabaseNode::modifiedTime() const
{
    return this->m_modified;
}

void DatabaseNode::setModified(const QDateTime &modified)
{
    if (this->m_modified != modified && modified.isValid())
    {
        this->m_modified = modified.toUTC();
    }
}

void DatabaseNode::setModified()
{
    this->m_modified = QDateTime::currentDateTime().toUTC();
    emit this->modified();
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
        this->m_uuid = uuid;
        this->setModified();
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
        this->m_title = title;
        this->setModified();
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
    Returns a value indicating whether the node is attached to a database.
 */
bool DatabaseNode::hasDatabase() const
{
    return dynamic_cast<Database*>(this->rootNode());
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

void DatabaseNode::fromXml(const QDomElement &element)
{
    // This function should NEVER be called from a parented DatabaseNode
    // It is private and designed to be called in DatabaseReader only
    Q_ASSERT(this->m_parent == NULL);

    this->setCreated(QDateTime::fromString(element.attribute(XML_CREATED), Qt::ISODate).toUTC());
    this->setAccessed(QDateTime::fromString(element.attribute(XML_ACCESSED), Qt::ISODate).toUTC());
    this->setModified(QDateTime::fromString(element.attribute(XML_MODIFIED), Qt::ISODate).toUTC());
    this->setUuid(element.attribute(XML_UUID));
    this->setTitle(element.attribute(XML_TITLE));
}

QDomElement DatabaseNode::toXml(QDomDocument &document) const
{
    QDomElement element = document.createElement("DatabaseNode");
    element.setAttribute(XML_CREATED, this->created().toUTC().toString(Qt::ISODate) + "Z");
    element.setAttribute(XML_ACCESSED, this->accessed().toUTC().toString(Qt::ISODate) + "Z");
    element.setAttribute(XML_MODIFIED, this->modifiedTime().toUTC().toString(Qt::ISODate) + "Z");
    element.setAttribute(XML_UUID, this->uuid().toString());
    element.setAttribute(XML_TITLE, this->title());
    return element;
}
