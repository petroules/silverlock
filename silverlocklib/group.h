#ifndef GROUP_H
#define GROUP_H

#include "silverlocklib_global.h"
#include "databasenode.h"
#include "searchparameters.h"
#include <QtCore>

class Entry;

class SILVERLOCKLIBSHARED_EXPORT Group : public DatabaseNode
{
    Q_OBJECT

    friend class Entry;

public:
    explicit Group(const QString &title = QString(), Group *parent = NULL);
    ~Group();
    const QList<Group*>& groups() const;
    const QList<Entry*>& entries() const;
    int countGroups() const;
    int countEntries() const;
    bool containsUuid(const QUuid &uuid) const;
    bool isGroup(const QUuid &uuid) const;
    bool isEntry(const QUuid &uuid) const;
    const Group* findGroup(const QUuid &uuid, bool includeThis = false) const;
    Group* findGroup(const QUuid &uuid, bool includeThis = false);
    Entry* findEntry(const QUuid &uuid) const;
    QList<Entry*> findEntries(const SearchParameters &params) const;
    QDomElement toXml(QDomDocument &document) const;

protected:
    void attachToList();
    void detachFromList();

private:
    Group* findGroupInternal(const QUuid &uuid) const;
    QList<Group*> m_groups;
    QList<Entry*> m_entries;
};

#endif // GROUP_H
