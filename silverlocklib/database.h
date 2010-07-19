#ifndef DATABASE_H
#define DATABASE_H

#include "silverlocklib_global.h"
#include "group.h"
#include "qversion.h"
#include <QtXml>

class SILVERLOCKLIBSHARED_EXPORT Database : public Group
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
    void setParentNode(Group *node);
    QDomElement toXml(QDomDocument &document) const;

protected:
    void attachToList();
    void detachFromList();

private:
    QString m_password;
};

#endif // DATABASE_H
