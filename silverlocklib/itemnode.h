#ifndef ITEMNODE_H
#define ITEMNODE_H

#include "databasenode.h"
#include <QtCore>

class GroupNode;

class SILVERLOCKLIBSHARED_EXPORT ItemNode : public virtual DatabaseNode
{
    Q_OBJECT

public:
    virtual ~ItemNode() { }
    GroupNode* rootNode() const;
    void setParentNode(GroupNode *node);
    bool isAttached() const;
    bool hasDatabase() const;

protected:
    explicit ItemNode();
    void detach();
    virtual void attachToList() = 0;
    virtual void detachFromList() = 0;

private:
    void attach();
};

#endif // ITEMNODE_H
