#ifndef DATABASE_H
#define DATABASE_H

#include "silverlocklib_global.h"
#include "group.h"
#include <QtCore>

class QDomDocument;
class QDomElement;
struct QVersion;

class SILVERLOCK_API Database : public Group
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
    int compression() const;
    void setCompression(int compression);
    void setParentNode(Group *node);
    QDomElement toXml(QDomDocument &document) const;

protected:
    void attachToList();
    void detachFromList();
    void fromXml(const QDomElement &element);

private:
    QString m_password;
    int m_compression;
};

#endif // DATABASE_H
