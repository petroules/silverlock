#include "databasenode.h"
#include "groupnode.h"

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

GroupNode* DatabaseNode::parentNode() const
{
    return this->m_parent;
}
