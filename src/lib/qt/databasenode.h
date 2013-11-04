#ifndef DATABASENODE_H
#define DATABASENODE_H

#include "silverlocklib_global.h"
#include "database_keys.h"
#include <QtCore>

class Group;
class QDomDocument;
class QDomElement;

class SILVERLOCK_API DatabaseNode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDateTime created READ created)
    Q_PROPERTY(QDateTime accessed READ accessed)
    Q_PROPERTY(QDateTime modified READ modified)
    Q_PROPERTY(QUuid uuid READ uuid WRITE setUuid)
    Q_PROPERTY(QString title READ title WRITE setTitle)

    friend class DatabaseReader;

public:
    virtual ~DatabaseNode() { }
    QDateTime created() const;
    QDateTime accessed() const;
    void setAccessed();
    QDateTime modified() const;
    QUuid uuid() const;
    QString title() const;
    void setTitle(const QString &title);
    Group* parentNode() const;
    virtual void setParentNode(Group *node);
    Group* rootNode() const;
    bool hasDatabase() const;
    virtual QDomElement toXml(QDomDocument &document) const;

    /*!
        Returns a value indicating whether the item has a parent node.
     */
    inline bool isAttached() const { return this->parentNode(); }

signals:
    /*!
        Emitted when the database node or any node in its tree is modified.

        \note Implementation note: when the database tree only needs to be notified of modification,
        emit this signal, however when the node data itself has been modified, instead use the
        \a setModified() method. It will emit the \a treeModified() signal.
     */
    void treeModified();

protected:
    explicit DatabaseNode(const QString &title = QString());
    void setUuid(QUuid uuid);
    void setModified();
    void setModified(const QDateTime &modified);
    void detach();
    virtual void fromXml(const QDomElement &element);

    /*!
        Adds the node to its parent's list of child nodes.
     */
    virtual void attachToList() = 0;

    /*!
        Removes the node from its parent's list of child nodes.
     */
    virtual void detachFromList() = 0;

    /*!
        See parentNode().
     */
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
