#ifndef GROUP_H
#define GROUP_H

#include "silverlocklib_global.h"
#include "groupnode.h"
#include "itemnode.h"
#include <QtCore>

class Database;
class Entry;

class SILVERLOCKLIBSHARED_EXPORT Group : public GroupNode, public ItemNode
{
    Q_OBJECT
    Q_PROPERTY(QUuid uuid READ uuid WRITE setUuid)
    Q_PROPERTY(QString title READ title WRITE setTitle)

public:
    explicit Group(const QString &title = QString(), GroupNode *parent = NULL);
    ~Group();
    QDomElement toXml(QDomDocument &document) const;

protected:
    void attachToList();
    void detachFromList();
};

#endif // GROUP_H
