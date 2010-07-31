#ifndef DATABASE_H
#define DATABASE_H

#include "silverlocklib_global.h"
#include <QtXml>
#include <liel.h>
#include "group.h"

class SILVERLOCKLIBSHARED_EXPORT Database : public Group
{
    Q_OBJECT
    Q_PROPERTY(QString password READ password WRITE setPassword)

    friend class DatabaseReader;

public:
    Database(const QString &title = QString(), const QString &password = QString());
    ~Database();
    static QVersion version();
    QString password() const;
    void setPassword(const QString &password);
    void setParentNode(Group *node);
    QDomElement toXml(QDomDocument &document) const;

protected:
    void attachToList();
    void detachFromList();
    void fromXml(const QDomElement &element);

private:
    QString m_password;
};

#endif // DATABASE_H
