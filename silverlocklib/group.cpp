#include "group.h"
#include "database.h"
#include "entry.h"
#include "database_keys.h"

/*!
    \class Group
    Represents a group of password entries in a database.

    The parent of a group cannot be \c NULL and an assertion failure will
    result if a \c NULL pointer is passed to any of the constructors.
 */

Group::Group(const QString &title, GroupNode *parent) :
    GroupNode(), ItemNode()
{
    this->setTitle(title);
    this->setParentNode(parent);
}

/*!
    Destroys the group, deleting all its child objects.

    The group's entries and subgroups are deleted upon deconstruction, which will recursively
    delete all subgroups and all entries contained within them. The group also removes itself
    from its parent group, if any.
 */
Group::~Group()
{
    this->detach();
}

void Group::attachToList()
{
    this->parentNode()->m_groups.append(this);
}

void Group::detachFromList()
{
    this->parentNode()->m_groups.removeAll(this);
}

QDomElement Group::toXml(QDomDocument &document) const
{
    QDomElement element = document.createElement(XML_GROUP);
    element.setAttribute(XML_UUID, this->uuid().toString());
    element.setAttribute(XML_TITLE, this->title());
    return element;
}
