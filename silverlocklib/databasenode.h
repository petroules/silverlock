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
    QDateTime created() const;
    QDateTime accessed() const;
    void setAccessed();
    QDateTime modifiedTime() const;
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
    virtual QDomElement toXml(QDomDocument &document) const;

signals:
    /*!
        Emitted when the database node or any node in its tree is modified.

        \note Implementation note: when the database tree needs to be notified of modification,
        emit this signal, however when the node data itself has been modified, instead use the
        \a setModified method. It will emit the \a modified() signal.
     */
    void modified();

protected:
    explicit DatabaseNode(const QString &title = QString());
    void setUuid(QUuid uuid);
    void setModified();
    void setModified(const QDateTime &modified);
    void detach();
    virtual void attachToList() = 0;
    virtual void detachFromList() = 0;
    virtual void fromXml(const QDomElement &element);
    Group *m_parent;

private:
    void setCreated(const QDateTime &created);
    void setAccessed(const QDateTime &accessed);
    void attach();
    QDateTime m_created;
    QDateTime m_accessed;
    QDateTime m_modified;
    QUuid m_uuid;
    QString m_title;
};

#endif // DATABASENODE_H
