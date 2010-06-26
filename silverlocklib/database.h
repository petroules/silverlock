#ifndef DATABASE_H
#define DATABASE_H

#include "silverlocklib_global.h"
#include "qversion.h"
#include <QtCore>
#include <QtXml>

class Entry;
class Group;

class SILVERLOCKLIBSHARED_EXPORT Database : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVersion version READ version)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString password READ password WRITE setPassword)

public:
    Database(const QString &name, const QString &password, QObject *parent = NULL);
    ~Database();
    static QVersion version();
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
    Group* findGroup(const QUuid &uuid, Group *const startingGroup) const;
    Entry* findEntry(const QUuid &uuid) const;
    Entry* findEntry(const QUuid &uuid, Group *const startingGroup) const;
    static Database* read(QIODevice &device, const QString &password);
    bool write(QIODevice &device) const;

private:
    void append(QDomDocument &document, QDomElement &element, Group *const group) const;

    QString m_name;
    QString m_password;
    Group *m_rootGroup;
};

#endif // DATABASE_H
