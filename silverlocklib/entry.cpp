#include "entry.h"
#include "group.h"
#include <climits>

/*!
    \class Entry
    The Entry class represents a password entry in a database.

    The parent of an entry cannot be \c NULL and an assertion failure will
    result if a \c NULL pointer is passed to any of the constructors.

    \sa Database, Group
 */

/*!
    Constructs a new entry with the specified title and belonging to the specified group.

    \param title The title of the entry.
    \param parent The group to which this entry will belong.
 */
Entry::Entry(const QString &title, Group *parent) :
    DatabaseNode(title)
{
    this->setParentNode(parent);
}

/*!
    Destroys the entry, removing itself from its parent group.
 */
Entry::~Entry()
{
    this->detach();
}

void Entry::attachToList()
{
    this->parentNode()->m_entries.append(this);
}

void Entry::detachFromList()
{
    this->parentNode()->m_entries.removeAll(this);
}

QUrl Entry::url() const
{
    return this->m_url;
}

void Entry::setUrl(const QUrl &url)
{
    if (this->m_url != url)
    {
        this->m_url = url;
        this->setModified();
    }
}

QString Entry::username() const
{
    return this->m_username;
}

void Entry::setUsername(const QString &username)
{
    if (this->m_username != username)
    {
        this->m_username = username;
        this->setModified();
    }
}

QString Entry::password() const
{
    return this->m_password;
}

void Entry::setPassword(const QString &password)
{
    if (this->m_password != password)
    {
        this->m_password = password;
        this->setModified();
    }
}

QString Entry::notes() const
{
    return this->m_notes;
}

void Entry::setNotes(const QString &notes)
{
    if (this->m_notes != notes)
    {
        this->m_notes = notes;
        this->setModified();
    }
}

const QMap<QString, QString>& Entry::recoveryInfo() const
{
    return this->m_recoveryInfo;
}

const QMap<QString, QString>& Entry::customFields() const
{
    return this->m_customFields;
}

void Entry::insertRecoveryInfo(const QString &key, const QString &value)
{
    // Blank questions are not accepted!
    if (key.isEmpty())
    {
        return;
    }

    this->m_recoveryInfo.insert(key, value);
    this->setModified();
}

int Entry::removeRecoveryInfo(const QString &key)
{
    int code = this->m_recoveryInfo.remove(key);

    // If at least one item was actually removed...
    if (code > 0)
    {
        this->setModified();
    }

    return code;
}

void Entry::clearRecoveryInfo()
{
    if (this->m_recoveryInfo.count() > 0)
    {
        this->m_recoveryInfo.clear();
        this->setModified();
    }
}

void Entry::insertCustomField(const QString &key, const QString &value)
{
    this->m_customFields.insert(key, value);
    this->setModified();
}

int Entry::removeCustomField(const QString &key)
{
    int code = this->m_customFields.remove(key);

    // If at least one item was actually removed...
    if (code > 0)
    {
        this->setModified();
    }

    return code;
}

void Entry::clearCustomFields()
{
    if (this->m_customFields.count() > 0)
    {
        this->m_customFields.clear();
        this->setModified();
    }
}

void Entry::fromXml(const QDomElement &element)
{
    DatabaseNode::fromXml(element);

    this->setUrl(element.attribute(XML_URL));
    this->setUsername(element.attribute(XML_USERNAME));
    this->setPassword(element.attribute(XML_PASSWORD));
    this->setNotes(element.attribute(XML_NOTES));

    // This function should NEVER be called from a parented DatabaseNode
    // It is private and designed to be called in DatabaseReader only
    // immediately after an Entry is instantiated, therefore they should
    // always be empty at this point
    Q_ASSERT(this->m_customFields.count() == 0);
    Q_ASSERT(this->m_recoveryInfo.count() == 0);

    // Here we'll process any subnodes of the entry containing additional information
    QDomNode entryNode = element.firstChild();
    while (!entryNode.isNull())
    {
        QDomElement entryElement = entryNode.toElement();
        if (!entryElement.isNull())
        {
            if (entryElement.tagName() == XML_CUSTOMFIELD)
            {
                this->m_customFields.insert(entryElement.attribute(XML_CFNAME), entryElement.attribute(XML_CFVALUE));
            }
            else if (entryElement.tagName() == XML_RECOVERYINFO)
            {
                this->m_recoveryInfo.insert(entryElement.attribute(XML_QUESTION), entryElement.attribute(XML_ANSWER));
            }
        }

        entryNode = entryNode.nextSibling();
    }
}

QDomElement Entry::toXml(QDomDocument &document) const
{
    QDomElement element = DatabaseNode::toXml(document);
    element.setTagName(XML_ENTRY);
    element.setAttribute(XML_URL, this->url().toString());
    element.setAttribute(XML_USERNAME, this->username());
    element.setAttribute(XML_PASSWORD, this->password());
    element.setAttribute(XML_NOTES, this->notes());

    // Add all the recovery info
    QMapIterator<QString, QString> i(this->recoveryInfo());
    while (i.hasNext())
    {
        i.next();
        QDomElement pair = document.createElement(XML_RECOVERYINFO);
        pair.setAttribute(XML_QUESTION, i.key());
        pair.setAttribute(XML_ANSWER, i.value());
        element.appendChild(pair);
    }

    // Add all the custom fields
    QMapIterator<QString, QString> j(this->customFields());
    while (j.hasNext())
    {
        j.next();
        QDomElement pair = document.createElement(XML_CUSTOMFIELD);
        pair.setAttribute(XML_CFNAME, j.key());
        pair.setAttribute(XML_CFVALUE, j.value());
        element.appendChild(pair);
    }

    return element;
}

/*!
    Creates a deep copy of the entry.

    The copy will not have all the same properties as the original.
    Specifically, a new UUID will be generated and the copy will be
    initialized without a parent node. The creation, access and modification
    dates will also be regenerated.
 */
Entry* Entry::createCopy() const
{
    Entry *entry = new Entry();
    entry->setTitle(this->title());
    entry->setUrl(this->url());
    entry->setUsername(this->username());
    entry->setPassword(this->password());
    entry->setNotes(this->notes());

    QMapIterator<QString, QString> i(this->recoveryInfo());
    while (i.hasNext())
    {
        i.next();
        entry->insertRecoveryInfo(i.key(), i.value());
    }

    QMapIterator<QString, QString> j(this->customFields());
    while (j.hasNext())
    {
        j.next();
        entry->insertCustomField(j.key(), j.value());
    }

    return entry;
}
