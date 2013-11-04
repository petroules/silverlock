#include "entry.h"
#include "group.h"
#include <QtXml>

/*!
    \class Entry

    The Entry class represents a password entry in a Database.

    \sa DatabaseNode, Database, Group
 */

/*!
    Constructs a new Entry with the specified title and parent.

    \param title The title, or name, of the entry.
    \param parent The parent group node to which this entry will belong.
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

/*!
    \property Entry::username

    This property holds the username field of the entry.
 */

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

/*!
    \property Entry::password

    This property holds the password field of the entry.
 */

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

/*!
    \property Entry::url

    This property holds the URL field of the entry.
 */

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

/*!
    \property Entry::notes

    This property holds the notes field of the entry.
 */

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

/*!
    Gets an immutable reference to the list of recovery questions and answers within the entry.
 */
const QMap<QString, QString>& Entry::recoveryInfo() const
{
    return this->m_recoveryInfo;
}

/*!
    Gets an immutable reference to the list of custom field names and values within the entry.
 */
const QMap<QString, QString>& Entry::customFields() const
{
    return this->m_customFields;
}

/*!
    Adds a recovery question and answer pair to the entry.

    \param question The question to add.
    \param answer The answer to add.

    If \a question is an empty string or a question and answer pair with the question text
    \a question already exists in the list, nothing is inserted and no modified signal is emitted.
 */
void Entry::insertRecoveryInfo(const QString &question, const QString &answer)
{
    // Blank questions are not accepted!
    if (question.isEmpty() || this->m_recoveryInfo.contains(question))
    {
        return;
    }

    this->m_recoveryInfo.insert(question, answer);
    this->setModified();
}

/*!
    Removes the recovery question and answer pair with the specified question text from the entry.

    The \a treeModified() signal is only emitted if at least one item was actually removed.

    \param question The question text of the question and answer pair to remove.
 */
int Entry::removeRecoveryInfo(const QString &question)
{
    int code = this->m_recoveryInfo.remove(question);

    // If at least one item was actually removed...
    if (code > 0)
    {
        this->setModified();
    }

    return code;
}

/*!
    Clears all recovery question and answer pairs from the entry.

    The \a treeModified() signal is only emitted if at least one item was actually removed.
 */
void Entry::clearRecoveryInfo()
{
    if (this->m_recoveryInfo.count() > 0)
    {
        this->m_recoveryInfo.clear();
        this->setModified();
    }
}

/*!
    Adds a custom field entry to the entry.

    \param key The name of the custom field.
    \param value The value of the custom field.

    If \a key is an empty string or a custom field entry with the field name \a key already
    exists in the list, nothing is inserted and no modified signal is emitted.
 */
void Entry::insertCustomField(const QString &key, const QString &value)
{
    // Blank keys are not accepted!
    if (key.isEmpty() || this->m_customFields.contains(key))
    {
        return;
    }

    this->m_customFields.insert(key, value);
    this->setModified();
}

/*!
    Removes the custom field entry with the specified field name from the entry.

    The \a treeModified() signal is only emitted if at least one item was actually removed.

    \param key The field name of the custom field entry to remove.
 */
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

/*!
    Clears all custom fields from the entry.

    The \a treeModified() signal is only emitted if at least one item was actually removed.
 */
void Entry::clearCustomFields()
{
    if (this->m_customFields.count() > 0)
    {
        this->m_customFields.clear();
        this->setModified();
    }
}

void Entry::attachToList()
{
    this->parentNode()->m_entries.append(this);
}

void Entry::detachFromList()
{
    this->parentNode()->m_entries.removeAll(this);
}

void Entry::fromXml(const QDomElement &element)
{
    DatabaseNode::fromXml(element);

    this->setUsername(element.attribute(XML_USERNAME));
    this->setPassword(element.attribute(XML_PASSWORD));
    this->setUrl(element.attribute(XML_URL));
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
    element.setAttribute(XML_USERNAME, this->username());
    element.setAttribute(XML_PASSWORD, this->password());
    element.setAttribute(XML_URL, this->url().toString());
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

    The copy will not have all the same properties as the original. Specifically:

    \li A new UUID will be generated.
    \li The copy will be initialized without a parent node.
    \li The creation, access and modification dates will be
    left at their defaults (current UTC date and time).
 */
Entry* Entry::createCopy() const
{
    Entry *entry = new Entry();
    entry->setTitle(this->title());
    entry->setUsername(this->username());
    entry->setPassword(this->password());
    entry->setUrl(this->url());
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
