#ifndef DATABASE_H
#define DATABASE_H

#include "silverlocklib_global.h"
#include "groupnode.h"
#include "qversion.h"
#include <QtCore>
#include <QtXml>

class Entry;
class Group;

class SILVERLOCKLIBSHARED_EXPORT Database : public GroupNode
{
    Q_OBJECT
    Q_PROPERTY(QVersion version READ version)
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString password READ password WRITE setPassword)

public:
    Database(const QString &name, const QString &password);
    ~Database();
    static QVersion version();
    QString password() const;
    void setPassword(const QString &password);
    QDomElement toXml(QDomDocument &document) const;

private:
    QString m_password;
};

#endif // DATABASE_H
