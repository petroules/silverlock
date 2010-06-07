#ifndef DATABASE_H
#define DATABASE_H
#define DATABASE_VERSION 1

#include "silverlocklib_global.h"
#include <QtCore>
#include <QtXml>
#include "entry.h"
#include "group.h"

class SILVERLOCKLIBSHARED_EXPORT Database
{
public:
    Database(const QString &name, const QString &password);
    ~Database();
    QString name() const;
    void setName(const QString &name);
    QString password() const;
    void setPassword(const QString &password);
    Group* rootGroup() const;
    int countGroups();
    int countEntries();
    bool isGroup(const QUuid &uuid) const;
    bool isEntry(const QUuid &uuid) const;
    Group* findGroup(const QUuid &uuid) const;
    Group* findGroup(const QUuid &uuid, Group *const currentGroup) const;
    Entry* findEntry(const QUuid &uuid) const;
    Entry* findEntry(const QUuid &uuid, Group *const currentGroup) const;
    static Database* read(QIODevice &device, const QString &password);
    bool write(QIODevice &device) const;
    void append(QDomDocument &document, QDomElement &element, Group *const group) const;

private:
    QString m_name;
    QString m_password;
    Group *m_rootGroup;
};

#endif // DATABASE_H
