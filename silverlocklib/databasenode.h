#ifndef DATABASENODE_H
#define DATABASENODE_H

#include "silverlocklib_global.h"
#include "database_keys.h"
#include <QtXml>

class Group;

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
    Group* parentNode() const;
    virtual void setParentNode(Group *node);
    Group* rootNode() const;
    bool hasDatabase() const;

    /*!
        Returns whether the item is attached to a parent node.
     */
    inline bool isAttached() const { return this->parentNode(); }
    virtual QDomElement toXml(QDomDocument &document) const = 0;

signals:
    /*!
        Emitted when the database node or any node in its tree is modified.
     */
    void modified();

protected:
    explicit DatabaseNode();
    void setUuid(QUuid uuid);
    void detach();
    virtual void attachToList() = 0;
    virtual void detachFromList() = 0;
    Group *m_parent;

private:
    void attach();
    QUuid m_uuid;
    QString m_title;
};

#endif // DATABASENODE_H
