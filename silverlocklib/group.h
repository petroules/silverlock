#ifndef GROUP_H
#define GROUP_H

#include "entry.h"
#include <QtCore>

class SILVERLOCKLIBSHARED_EXPORT Group
{
public:
    Group(const QString &name);
    ~Group();
    QUuid uuid() const;
    QString name() const;
    void setName(const QString &name);
    QList<Group*>& groups();
    QList<Entry*>& entries();
    int countGroups();
    int countEntries();

private:
    QUuid m_uuid;
    QString m_name;
    QList<Group*> m_groups;
    QList<Entry*> m_entries;
};

#endif // GROUP_H
