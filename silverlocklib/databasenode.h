#ifndef DATABASENODE_H
#define DATABASENODE_H

#include "silverlocklib_global.h"
#include <QtXml>

class GroupNode;

class SILVERLOCKLIBSHARED_EXPORT DatabaseNode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUuid uuid READ uuid WRITE setUuid)
    Q_PROPERTY(QString title READ title WRITE setTitle)

    friend class DatabaseReader;

public:
    virtual ~DatabaseNode() { }
    QUuid uuid() const;
    QString title() const;
    void setTitle(const QString &title);
    GroupNode* parentNode() const;
    virtual QDomElement toXml(QDomDocument &document) const = 0;

signals:
    /*!
        Emitted when the database node or any node in its tree is modified.
     */
    void modified();

protected:
    explicit DatabaseNode();
    void setUuid(QUuid uuid);
    GroupNode *m_parent;

private:
    QUuid m_uuid;
    QString m_title;
};

#endif // DATABASENODE_H
