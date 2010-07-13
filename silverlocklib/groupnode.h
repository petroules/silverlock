#ifndef GROUPNODE_H
#define GROUPNODE_H

#include "silverlocklib_global.h"
#include "databasenode.h"
#include <QtCore>

class Group;
class Entry;

class SILVERLOCKLIBSHARED_EXPORT GroupNode : public virtual DatabaseNode
{
    Q_OBJECT

    friend class Group;
    friend class Entry;

public:
    virtual ~GroupNode();
    const QList<Group*>& groups() const;
    const QList<Entry*>& entries() const;
    int countGroups() const;
    int countEntries() const;
    bool containsUuid(const QUuid &uuid) const;
    bool isGroup(const QUuid &uuid) const;
    bool isEntry(const QUuid &uuid) const;
    Group* findGroup(const QUuid &uuid) const;
    Entry* findEntry(const QUuid &uuid) const;

protected:
    explicit GroupNode();

private:
    QList<Group*> m_groups;
    QList<Entry*> m_entries;
};

#endif // GROUPNODE_H
