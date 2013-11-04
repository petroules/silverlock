#include "database.h"
#include "database_keys.h"
#include <QtXml>

/*!
    \class Database

    The Database class is used as the root node of a tree structure in which groups and password
    entries are stored. See DatabaseNode for more information.

    \note Please refer to the documentation for version() for information regarding versioning.

    \sa version(), DatabaseNode, Group, Entry
 */

/*!
    Constructs a new Database with the specified title and password.

    \param title The title, or name, of the database.
    \param password The database password. See \a password.
 */
Database::Database(const QString &title, const QString &password)
    : Group(title), m_password(password), m_compression(-1)
{
}

/*!
    Destroys the database, deleting all its child objects.

    The database's child nodes are deleted upon destruction, which will recursively delete all
    subgroups and all entries contained within them.
 */
Database::~Database()
{
}

/*!
    Gets the version of the Silverlock database XML format this class supports writing to.

    All versions of silverlocklib support reading from any version including and previous to the
    one it explicitly supports, for maximum backward compatibility. The Database class will always
    write to the latest format it supports by default. Currently there is no functionality available
    nor planned to support writing of old formats, to encourage users to stay up to date with the
    latest technology available.

    This decision is subject to change.
 */
QString Database::version()
{
    return QLatin1String("1.0");
}

/*!
    \property Database::password

    This property holds the database password.

    This value is used as part of the encryption key to the database
    when it is written to a file or other storage mechanism.
 */

QString Database::password() const
{
    return this->m_password;
}

void Database::setPassword(const QString &password)
{
    if (this->m_password != password)
    {
        this->m_password = password;
        this->setModified();
    }
}

/*!
    \property Database::compression

    This property holds the database compression level.

    -1 specifies default compression, 0 specifies no compression,
    and 1 through 9 specify custom compression levels, 1 being
    the least and 9 being the most. Attempts to set values outside
    this range will result in the value being set to the default (-1).
 */
int Database::compression() const
{
    return this->m_compression;
}

void Database::setCompression(int compression)
{
    if (this->m_compression != compression)
    {
        // If the compression is outside the proper range...
        if (compression < -1 || compression > 9)
        {
            // ...default to default compression
            compression = -1;
        }

        this->m_compression = compression;
        this->setModified();
    }
}

/*!
    This method does nothing; it is overridden from its base implementation
    to have no effect as Database classes cannot have parent nodes.
 */
void Database::setParentNode(Group *node)
{
    Q_UNUSED(node);
}

/*!
    \copydoc Database::setParentNode()
 */
void Database::attachToList()
{
}

/*!
    \copydoc Database::setParentNode()
 */
void Database::detachFromList()
{
}

void Database::fromXml(const QDomElement &element)
{
    DatabaseNode::fromXml(element);

    this->setPassword(element.attribute(XML_DBPASSWORD));
    this->setCompression(element.attribute(XML_COMPRESSION, QString::number(-1)).toInt());
}

QDomElement Database::toXml(QDomDocument &document) const
{
    QDomElement element = Group::toXml(document);
    element.setTagName(XML_DATABASE);
    element.setAttribute(XML_VERSION, Database::version());
    element.setAttribute(XML_DBPASSWORD, this->m_password);
    element.setAttribute(XML_COMPRESSION, this->m_compression);
    return element;
}
