#include "group.h"

Group::Group(const QString &name)
    : m_uuid(QUuid::createUuid()), m_name(name)
{
}

Group::~Group()
{
    for (int i = 0; i < this->m_groups.length(); i++)
    {
        delete this->m_groups[i];
        this->m_groups[i] = NULL;
    }

    for (int i = 0; i < this->m_entries.length(); i++)
    {
        delete this->m_entries[i];
        this->m_entries[i] = NULL;
    }
}

QUuid Group::uuid() const
{
    return this->m_uuid;
}

QString Group::name() const
{
    return this->m_name;
}

void Group::setName(const QString &name)
{
    this->m_name = name;
}

QList<Group*>& Group::groups()
{
    return this->m_groups;
}

QList<Entry*>& Group::entries()
{
    return this->m_entries;
}

int Group::countGroups()
{
    int total = 0;

    for (int i = 0; i < this->groups().count(); i++)
    {
        total += this->groups().at(i)->countGroups();
    }

    return total;
}

int Group::countEntries()
{
    int total = 0;

    for (int i = 0; i < this->groups().count(); i++)
    {
        total += this->groups().at(i)->countEntries();
    }

    return total + this->entries().count();
}
