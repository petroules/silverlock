#include "database.h"
#include "database_keys.h"

/*!
    \class Database
    The Database class is a tree structure in which Silverlock groups and entries are stored.

    Data is stored in a tree format. Each Database instance contains a root \link Group \endlink
    which can contain 0 or more entries (see \link Entry \endlink) and 0 or more groups, which can
    each contain additional groups and entries, and so on.

    Each group and entry are assigned a UUID upon construction to ensure that there is a way to
    uniquely reference a group or entry in the database. Uniquity is shared between groups and
    entries in a database - no group will have the same UUID as an entry, and due to the nature
    of UUIDs it is extremely unlikely that there will be a collision between databases, however
    this cannot be gauranteed.

    \note Please refer to the documentation for \link version() \endlink for important information
    regarding versioning.

    \sa version(), Group, Entry
 */

/*!
    Constructs a new Database with the specified name and password.

    When the database is constructed, a root group is automatically created using the
    name of the database. The name of the database is retrieved from its root group.

    \param title The name of the database, which is also the name of the root group.
    \param password The password used to encrypted the database contents when it is written to a storage medium.
    \param parent The parent object of the Database. Setting \a parent to \c NULL constructs an object with no parent.
 */
Database::Database(const QString &title, const QString &password)
    : Group(), m_password(password)
{
    this->setTitle(title);
}

/*!
    Destroys the database, deleting all its child objects.

    The database's root group is deleted upon deconstruction, which will recursively delete all
    subgroups and all entries contained within them.
 */
Database::~Database()
{
}

/*!
    Gets the version of the Silverlock database XML format this class supports writing to.

    The \link version() \endlink method returns a QString determining the version of the Silverlock
    database format this class supports writing to. All versions of silverlocklib support reading
    from any version including and previous to the one it explicitly supports, for maximum backward
    compatibility. Minor changes in database format are implemented using simple conditionals; in
    the case of a more extensive change to the format, internal classes will be created that will be
    called as per the version of the database that needs to be read. The Database class will always
    write to the latest format it supports by default. Currently there is no functionality available
    nor planned to support writing of old formats, to encourage users to stay updated. This decision
    is subject to change.
 */
QVersion Database::version()
{
    return QVersion("1.0");
}

/*!
    \property Database::password
    This property holds the password used to encrypt the database contents when it is written to a storage medium.
    \sa password(), setPassword(const QString &password)
 */

/*!
    \internal
 */
QString Database::password() const
{
    return this->m_password;
}

/*!
    \internal
 */
void Database::setPassword(const QString &password)
{
    this->m_password = password;
}

void Database::setParentNode(Group *node)
{
    Q_UNUSED(node);
}

void Database::attachToList()
{
}

void Database::detachFromList()
{
}

QDomElement Database::toXml(QDomDocument &document) const
{
    QDomElement element = document.createElement(XML_DATABASE);
    element.setAttribute(XML_UUID, this->uuid());
    element.setAttribute(XML_TITLE, this->title());
    element.setAttribute(XML_VERSION, Database::version().toString());
    element.setAttribute(XML_DBPASSWORD, this->m_password);
    return element;
}
