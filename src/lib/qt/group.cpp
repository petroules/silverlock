#include "group.h"
#include "entry.h"
#include <QtXml>

/*!
    \class Group

    The Group class represents a group of password entries in a Database.

    \sa DatabaseNode, Database, Entry
 */

/*!
    Constructs a new Group with the specified title and parent.

    \param title The title, or name, of the group.
    \param parent The parent group node to which this group will belong.
 */
Group::Group(const QString &title, Group *parent) :
    DatabaseNode(title)
{
    this->setParentNode(parent);
}

/*!
    Destroys the group, deleting all its child objects.

    The group's entries and subgroups are deleted upon deconstruction, which will recursively
    delete all subgroups and all entries contained within them. The group also removes itself
    from its parent group, if any.
 */
Group::~Group()
{
    this->detach();

    foreach (Entry *entry, this->entries())
    {
        delete entry;
    }

    foreach (Group *subgroup, this->groups())
    {
        delete subgroup;
    }
}

/*!
    Gets an immutable reference to the list of this group's child groups.
 */
const QList<Group*>& Group::groups() const
{
    return this->m_groups;
}

/*!
    Gets an immutable reference to the list of this group's child entries.
 */
const QList<Entry*> Group::entries(bool recursive) const
{
    QList<Entry*> entries;
    entries.append(this->m_entries);

    if (recursive)
    {
        foreach (Group *subgroup, this->groups())
        {
            entries.append(subgroup->entries(true));
        }
    }

    return entries;
}

/*!
    Gets the total number of groups in the group node, including subgroups.

    This method recursively queries each group for its number of subgroups, starting with but not
    counting the current group. If you want to get the number of groups in the current group only
    (non-recursive), call the \c count() method of groups().

    \sa countEntries()
 */
int Group::countGroups() const
{
    int total = 0;

    foreach (Group *group, this->groups())
    {
        total += group->countGroups();
    }

    return total + this->groups().count();
}

/*!
    Gets the total number of entries in the group node, including those in subgroups.

    This method recursively queries each group for its number of entries, starting with but not
    counting the current group. If you want to get the number of entries in the current group only
    (non-recursive), call the \c count() method of entries().

    \sa countGroups()
 */
int Group::countEntries() const
{
    int total = 0;

    foreach (Group *group, this->groups())
    {
        total += group->countEntries();
    }

    return total + this->entries().count();
}

/*!
    Determines whether the database contains a group or entry identified by the specified UUID.

    This method returns \c true if a group or entry exists in this database that is identified by
    \a uuid. If no such group or entry exists, this method returns \c false.

    \param uuid The UUID to search for.
 */
bool Group::containsUuid(const QUuid &uuid) const
{
    return this->isGroup(uuid) || this->isEntry(uuid);
}

/*!
    Determines whether the database contains a group identified by the specified UUID.

    This method returns \c true if a group exists in this database that is identified by \a uuid.
    If no such group exists, or an object exists but is not a child or descendant of the root group
    of this database (or is an entry), this method returns \c false.

    \param uuid The UUID to search for.
 */
bool Group::isGroup(const QUuid &uuid) const
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
bool Group::isEntry(const QUuid &uuid) const
{
    return this->findEntry(uuid) != NULL;
}

/*!
    Retrieves a pointer to the group contained in this group node with the specified UUID.

    If no group identified by \a uuid exists, a \c NULL pointer is returned.

    \param uuid The UUID to search for.
    \param includeThis Whether to include this group node in the search. If \a uuid is equal to
    the UUID of this group node, the group node itself will be returned. Otherwise only children
    and descendants can possibly be returned.
 */
const Group* Group::findGroup(const QUuid &uuid, bool includeThis) const
{
    if (includeThis && this->uuid() == uuid)
    {
        return this;
    }
    else
    {
        return this->findGroupInternal(uuid);
    }
}

/*!
    \overload Group::findGroup(const QUuid&, bool) const
 */
Group* Group::findGroup(const QUuid &uuid, bool includeThis)
{
    if (includeThis && this->uuid() == uuid)
    {
        return this;
    }
    else
    {
        return this->findGroupInternal(uuid);
    }
}

/*!
    \internal
 */
Group* Group::findGroupInternal(const QUuid &uuid) const
{
    foreach (Group *group, this->groups())
    {
        // If this group is a match, return it
        if (group->uuid() == uuid)
        {
            return group;
        }
        else
        {
            // Otherwise search its children
            Group *candidate = group->findGroup(uuid, true);
            if (candidate && candidate->uuid() == uuid)
            {
                return candidate;
            }
        }
    }

    return NULL;
}

/*!
    Retrieves a pointer to the entry contained in this group node with the specified UUID.

    If no entry identified by \a uuid exists, a \c NULL pointer is returned.

    \param uuid The UUID to search for.
 */
Entry* Group::findEntry(const QUuid &uuid) const
{
    foreach (Entry *entry, this->entries())
    {
        if (entry->uuid() == uuid)
        {
            return entry;
        }
    }

    foreach (Group *group, this->groups())
    {
        Entry *candidate = group->findEntry(uuid);
        if (candidate && candidate->uuid() == uuid)
        {
            return candidate;
        }
    }

    return NULL;
}

/*!
    Returns a list of entries matching the specified search parameters.

    \param params The search parameters to follow.
 */
QList<Entry*> Group::findEntries(const SearchParameters &params) const
{
    if (!params.fieldsSelected())
    {
        return QList<Entry*>();
    }

    QRegExp regex = QRegExp(params.searchPattern, params.caseSensitiveEnum());
    QList<Entry*> entries;

    foreach (Entry *entry, this->entries())
    {
        QList<QString> fields = params.getDataList(*entry);
        foreach (QString field, fields)
        {
            if (params.useRegex)
            {
                if (!entries.contains(entry) && field.contains(regex))
                {
                    entries.append(entry);

                    // Break out of fields loop so we go to the next entry
                    break;
                }
            }
            else
            {
                if (!entries.contains(entry) && field.contains(params.searchPattern, params.caseSensitiveEnum()))
                {
                    entries.append(entry);

                    // Break out of fields loop so we go to the next entry
                    break;
                }
            }
        }
    }

    foreach (Group *group, this->groups())
    {
        entries.append(group->findEntries(params));
    }

    return entries;
}

void Group::attachToList()
{
    this->parentNode()->m_groups.append(this);
}

void Group::detachFromList()
{
    this->parentNode()->m_groups.removeAll(this);
}

QDomElement Group::toXml(QDomDocument &document) const
{
    QDomElement element = DatabaseNode::toXml(document);
    element.setTagName(XML_GROUP);
    return element;
}
