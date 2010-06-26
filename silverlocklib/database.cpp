#include "database.h"
#include "entry.h"
#include "group.h"

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

    When the database is constructed, a root group is automatically created
    with the same name as the database. Changing the name of the database
    also changes the name of the root group.

    \param name The name of the database, which will also be the name of the root group.
    \param password The password used to encrypted the database contents when it is written to a storage medium.
    \param parent The parent object of the Database. Setting \a parent to \c NULL constructs an object with no parent.
 */
Database::Database(const QString &name, const QString &password, QObject *parent)
    : QObject(parent), m_name(name), m_password(password), m_rootGroup(new Group(name))
{
}

/*!
    Destroys the database, deleting all its child objects.

    The database's root group is deleted upon deconstruction, which will recursively delete all
    subgroups and all entries contained within them.
 */
Database::~Database()
{
    delete this->m_rootGroup;
    this->m_rootGroup = NULL;
}

/*!
    Gets the version of the Silverlock database XML format this class supports writing to.

    The \link version() \endlink method returns a QString determining the version of the Silverlock
    database format this class supports writing to. All versions of silverlocklib support reading
    from any version including and previous to the one it explicitly supports, for maximum backward
    compatibility. Minor changes in database format are implemented using simple conditionals; in the
    case of a more extensive change to the format, internal classes will be created that will be
    called as per the version of the database that needs to be read. The Database class will always write
    to the latest format it supports by default. Currently there is no functionality available nor planned
    to support writing of old formats, to encourage users to stay updated. This decision is subject to change.
 */
QVersion Database::version()
{
    return QVersion("1.0");
}

/*!
    \property Database::name
    This property holds the name of the database, which is also the name of the database's root group.
    \sa name(), setName(const QString &name)
 */

/*!
    \internal
 */
QString Database::name() const
{
    return this->m_name;
}

/*!
    \internal
 */
void Database::setName(const QString &name)
{
    this->m_name = name;
    this->m_rootGroup->setName(name);
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

/*!
    Gets the root group node of the database.

    This is the root node of the database's tree; all subgroups and entries are children or descendants of it.
 */
Group* Database::rootGroup() const
{
    return this->m_rootGroup;
}

/*!
    Gets the total number of groups in the database, including subgroups.

    This method recursively queries each group's number of subgroups, starting with the database's root group.
    If you want to get the number of groups in the root group only (non-recursive), call
    Database::rootGroup()::groups()::count().

    \sa countEntries()
 */
int Database::countGroups()
{
    int total = 0;

    for (int i = 0; i < this->rootGroup()->groups().count(); i++)
    {
        total += this->rootGroup()->groups().at(i)->countGroups();
    }

    return total;
}

/*!
    Gets the total number of entries in the database, including those in subgroups.

    This method recursively queries each group for its number of entries, starting with the database's root group.
    If you want to get the number of entries in the root group only (non-recursive), call
    Database::rootGroup()::entries()::count().

    \sa countGroups()
 */
int Database::countEntries()
{
    int total = 0;

    for (int i = 0; i < this->rootGroup()->groups().count(); i++)
    {
        total += this->rootGroup()->groups().at(i)->countEntries();
    }

    return total + this->rootGroup()->entries().count();
}

/*!
    Determines whether the database contains a group identified by the specified UUID.

    This method returns \c true if a group exists in this database that is identified by \a uuid.
    If no such group exists, or an object exists but is not a child or descendant of the root group
    of this database (or is an entry), this method returns \c false.

    \param uuid The UUID to search for.
 */
bool Database::isGroup(const QUuid &uuid) const
{
    return this->findGroup(uuid) != NULL;
}

/*!
    Determines whether the database contains an entry identified by the specified UUID.

    This method returns \c true if an entry exists in this database that is identified by \a uuid.
    If no such entry exists, or an object exists but is not a child or descendant of the root group
    of this database (or is a group), this method returns \c false.

    \param uuid The UUID to search for.
 */
bool Database::isEntry(const QUuid &uuid) const
{
    return this->findEntry(uuid) != NULL;
}

/*!
    Retrieves a pointer to the group contained in this database with the specified UUID.

    If no such group exists, a \c NULL pointer is returned.

    \param uuid The UUID to search for.
 */
Group* Database::findGroup(const QUuid &uuid) const
{
    return this->findGroup(uuid, this->m_rootGroup);
}

/*!
    Retrieves a pointer to the group contained in this database with the specified UUID,
    starting the recursive search at \a startingGroup.

    The recursive search starts from the group specified by \a startingGroup. If \a startingGroup
    is not contained within this database, a \c NULL pointer is returned.

    If no group identified by \a uuid exists, a \c NULL pointer is returned.

    \param uuid The UUID to search for.
    \param startingGroup The group to start the recursive search at.
 */
Group* Database::findGroup(const QUuid &uuid, Group *const startingGroup) const
{
    if (startingGroup == NULL || !this->isGroup(startingGroup->uuid()))
    {
        return NULL;
    }

    if (startingGroup->uuid() == uuid)
    {
        return startingGroup;
    }

    for (int i = 0; i < startingGroup->groups().count(); i++)
    {
        Group* candidate = this->findGroup(uuid, startingGroup->groups().at(i));
        if (candidate != NULL && candidate->uuid() == uuid)
        {
            return candidate;
        }
    }

    return NULL;
}

/*!
    Retrieves a pointer to the entry contained in this database with the specified UUID.

    If no such entry exists, a \c NULL pointer is returned.

    \param uuid The UUID to search for.
 */
Entry* Database::findEntry(const QUuid &uuid) const
{
    return this->findEntry(uuid, this->m_rootGroup);
}

/*!
    Retrieves a pointer to the entry contained in this database with the specified UUID,
    starting the recursive search at \a startingGroup.

    The recursive search starts from the group specified by \a startingGroup. If \a startingGroup
    is not contained within this database, a \c NULL pointer is returned.

    If no entry identified by \a uuid exists, a \c NULL pointer is returned.

    \param uuid The UUID to search for.
    \param startingGroup The group to start the recursive search at.
 */
Entry* Database::findEntry(const QUuid &uuid, Group *const startingGroup) const
{
    if (startingGroup == NULL || !this->isGroup(startingGroup->uuid()))
    {
        return NULL;
    }

    for (int i = 0; i < startingGroup->entries().count(); i++)
    {
        Entry* entry = startingGroup->entries().at(i);
        if (entry->uuid() == uuid)
        {
            return entry;
        }
    }

    for (int i = 0; i < startingGroup->groups().count(); i++)
    {
        Entry* candidate = this->findEntry(uuid, startingGroup->groups().at(i));
        if (candidate != NULL && candidate->uuid() == uuid)
        {
            return candidate;
        }
    }

    return NULL;
}

#define XML_DATABASE "Database"
#define XML_VERSION "Version"
#define XML_DBNAME "Name"
#define XML_DBPASSWORD "Password"

#define XML_UUID "UUID"

#define XML_GROUP "Group"
#define XML_GNAME "Name"

#define XML_ENTRY "Entry"
#define XML_TITLE "Title"
#define XML_URL "URL"
#define XML_USERNAME "Username"
#define XML_PASSWORD "Password"
#define XML_EMAILADDRESS "EmailAddress"
#define XML_NOTES "Notes"

#define XML_RECOVERYINFO "RecoveryInfo"
#define XML_QAPAIR "QuestionAnswerPair"
#define XML_QUESTION "Question"
#define XML_ANSWER "Answer"

#define XML_ADDITIONALDATA "AdditionalData"
#define XML_ADPAIR "AdditionalDataPair"
#define XML_ADNAME "Name"
#define XML_ADVALUE "Value"

/*!
    Reads a database from the specified QIODevice and returns a pointer to a Database,
    using \a password as the key to decrypt the contents.

    Databases are encrypted using 256-bit AES.

    The method first checks if the QIODevice is open and readable. If so, it proceeds
    to decrypt the contents using \a password as the key and then read the XML tree
    into a Database object. If the device is not open or readable, the decryption
    fails, or there is a versioning problem, the method returns a \c NULL pointer.

    For error information, see \link lastReadError() \endlink.
 */
Database* Database::read(QIODevice &device, const QString &password)
{
    if (device.isOpen() && device.isReadable())
    {
        QDomDocument doc(XML_DATABASE);
        if (doc.setContent(&device))
        {
            QDomElement root = doc.documentElement();
            if (root.tagName() == XML_DATABASE && QVersion(root.attribute(XML_VERSION)) == Database::version())
            {
                QDomNode n = root.firstChild();
                /*while (!n.isNull())
                {
                    QDomElement e = n.toElement();
                    if (!e.isNull())
                    {
                        if (e.tagName() == "contact")
                        {
                            Contact c;

                            c.name = e.attribute( "name", "" );
                            c.phone = e.attribute( "phone", "" );
                            c.eMail = e.attribute( "email", "" );

                            QMessageBox::information( 0, "Contact", c.name + "\n" + c.phone + "\n" + c.eMail );
                        }
                    }

                    n = n.nextSibling();
                }*/
            }
        }
    }

    return NULL;
}

/*!
    Writes the database to the specified QIODevice and returns whether it succeeded.

    The database is written in an XML format encrypted with 256-bit AES.

    The method first checks if the QIODevice is open and writable. If so, it proceeds
    to write the database contents to the device with a 4 character tab width. If the
    device is not open or writable, the method immediately returns false.
 */
bool Database::write(QIODevice &device) const
{
    QDomDocument document(XML_DATABASE);

    QDomElement dbName = document.createElement(XML_DATABASE);
    dbName.setAttribute(XML_VERSION, Database::version().toString());
    dbName.setAttribute(XML_DBNAME, this->m_name);
    dbName.setAttribute(XML_DBPASSWORD, this->m_password);
    document.appendChild(dbName);

    this->append(document, dbName, this->m_rootGroup);

    if (device.isOpen() && device.isWritable())
    {
        QTextStream ts(&device);
        ts << document.toString(4);
        return true;
    }

    return false;
}

/*!
    Writes \a group to the \a document as a child of \a element.

    \param document The QDomDocument representing the XML document to write data to.
    \param element The element which \a group should have its element created as a child of.
    \param group The Group whose contents are to be appended to the XML document.
 */
void Database::append(QDomDocument &document, QDomElement &element, Group *const group) const
{
    if (group == NULL)
    {
        return;
    }

    QDomElement cat = document.createElement(XML_GROUP);
    cat.setAttribute(XML_UUID, group->uuid().toString());
    cat.setAttribute(XML_GNAME, group->name());
    element.appendChild(cat);

    for (int i = 0; i < group->groups().count(); i++)
    {
        this->append(document, cat, group->groups().at(i));
    }

    for (int i = 0; i < group->entries().count(); i++)
    {
        Entry* account = group->entries().at(i);
        QDomElement acc = document.createElement(XML_ENTRY);
        acc.setAttribute(XML_UUID, account->uuid().toString());
        acc.setAttribute(XML_TITLE, account->title());
        acc.setAttribute(XML_URL, account->url().toString());
        acc.setAttribute(XML_USERNAME, account->username());
        acc.setAttribute(XML_PASSWORD, account->password());
        acc.setAttribute(XML_EMAILADDRESS, account->emailAddress());
        acc.setAttribute(XML_NOTES, account->notes());

        QDomElement recovery = document.createElement(XML_RECOVERYINFO);
        acc.appendChild(recovery);

        QHashIterator<QString, QString> i(account->recoveryInfo());
        while (i.hasNext())
        {
            i.next();
            QDomElement pair = document.createElement(XML_QAPAIR);
            pair.setAttribute(XML_QUESTION, i.key());
            pair.setAttribute(XML_ANSWER, i.value());
            recovery.appendChild(pair);
        }

        QDomElement additional = document.createElement(XML_ADDITIONALDATA);
        acc.appendChild(additional);

        QHashIterator<QString, QString> j(account->additionalData());
        while (j.hasNext())
        {
            j.next();
            QDomElement pair = document.createElement(XML_ADPAIR);
            pair.setAttribute(XML_ADNAME, j.key());
            pair.setAttribute(XML_ADVALUE, j.value());
            additional.appendChild(pair);
        }

        cat.appendChild(acc);
    }
}
