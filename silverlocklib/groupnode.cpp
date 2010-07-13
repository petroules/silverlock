#include "groupnode.h"
#include "entry.h"
#include "group.h"

GroupNode::GroupNode() :
    DatabaseNode()
{
}

GroupNode::~GroupNode()
{
    foreach (Entry *entry, this->entries())
    {
        delete entry;
    }

    foreach (Group *subgroup, this->groups())
    {
        delete subgroup;
    }
}

const QList<Group*>& GroupNode::groups() const
{
    return this->m_groups;
}

const QList<Entry*>& GroupNode::entries() const
{
    return this->m_entries;
}

/*!
    Gets the total number of groups in the group node, including subgroups.

    This method recursively queries each group for its number of subgroups, starting with but not
    counting the current group. If you want to get the number of groups in the current group only
    (non-recursive), call \link groups().count() \endlink.

    \sa countEntries()
 */
int GroupNode::countGroups() const
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
    (non-recursive), call \link entries().count() \endlink.

    \sa countGroups()
 */
int GroupNode::countEntries() const
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
bool GroupNode::containsUuid(const QUuid &uuid) const
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
bool GroupNode::isGroup(const QUuid &uuid) const
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
bool GroupNode::isEntry(const QUuid &uuid) const
{
    return this->findEntry(uuid) != NULL;
}

/*!
    Retrieves a pointer to the group contained in this group node with the specified UUID.

    The recursive search starts from the group specified by \a startingGroup. If \a startingGroup
    is not contained within this database, a \c NULL pointer is returned.

    If no group identified by \a uuid exists, a \c NULL pointer is returned.

    \param uuid The UUID to search for.
    \param includeThis Whether to include this group node in the search. If \a uuid is equal to
    the UUID of this group node, the group node itself will be returned. Otherwise TODO TODO
 */
Group* GroupNode::findGroup(const QUuid &uuid) const
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
            Group *candidate = group->findGroup(uuid);
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

    The recursive search starts from the group specified by \a startingGroup. If \a startingGroup
    is not contained within this database, a \c NULL pointer is returned.

    If no entry identified by \a uuid exists, a \c NULL pointer is returned.

    \param uuid The UUID to search for.
 */
Entry* GroupNode::findEntry(const QUuid &uuid) const
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
