#include "entry.h"
#include "database.h"
#include "group.h"
#include "database_keys.h"

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
Entry::Entry(const QString &title, GroupNode *parent) :
    ItemNode()
{
    this->setTitle(title);
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
        emit this->modified();
        this->m_url = url;
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
        emit this->modified();
        this->m_username = username;
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
        emit this->modified();
        this->m_password = password;
    }
}

QString Entry::emailAddress() const
{
    return this->m_emailAddress;
}

void Entry::setEmailAddress(const QString &emailAddress)
{
    if (this->m_emailAddress != emailAddress)
    {
        emit this->modified();
        this->m_emailAddress = emailAddress;
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
        emit this->modified();
        this->m_notes = notes;
    }
}

const QHash<QString, QString>& Entry::recoveryInfo() const
{
    return this->m_recoveryInfo;
}

const QHash<QString, QString>& Entry::additionalData() const
{
    return this->m_additionalData;
}

void Entry::insertRecoveryInfo(const QString &key, const QString &value)
{
    this->m_recoveryInfo.insert(key, value);
    emit this->modified();
}

int Entry::removeRecoveryInfo(const QString &key)
{
    int code = this->m_recoveryInfo.remove(key);

    // If at least one item was actually removed...
    if (code > 0)
    {
        emit this->modified();
    }

    return code;
}

void Entry::clearRecoveryInfo()
{
    if (this->m_recoveryInfo.count() > 0)
    {
        this->m_recoveryInfo.clear();
        emit this->modified();
    }
}

void Entry::insertAdditionalData(const QString &key, const QString &value)
{
    this->m_additionalData.insert(key, value);
    emit this->modified();
}

int Entry::removeAdditionalData(const QString &key)
{
    int code = this->m_additionalData.remove(key);

    // If at least one item was actually removed...
    if (code > 0)
    {
        emit this->modified();
    }

    return code;
}

void Entry::clearAdditionalData()
{
    if (this->m_additionalData.count() > 0)
    {
        this->m_additionalData.clear();
        emit this->modified();
    }
}

QDomElement Entry::toXml(QDomDocument &document) const
{
    QDomElement element = document.createElement(XML_ENTRY);
    element.setAttribute(XML_UUID, this->uuid().toString());
    element.setAttribute(XML_TITLE, this->title());
    element.setAttribute(XML_URL, this->url().toString());
    element.setAttribute(XML_USERNAME, this->username());
    element.setAttribute(XML_PASSWORD, this->password());
    element.setAttribute(XML_EMAILADDRESS, this->emailAddress());
    element.setAttribute(XML_NOTES, this->notes());

    // Add all the recovery info
    QHashIterator<QString, QString> i(this->recoveryInfo());
    while (i.hasNext())
    {
        i.next();
        QDomElement pair = document.createElement(XML_RECOVERYINFO);
        pair.setAttribute(XML_QUESTION, i.key());
        pair.setAttribute(XML_ANSWER, i.value());
        element.appendChild(pair);
    }

    // Add all the additional data
    QHashIterator<QString, QString> j(this->additionalData());
    while (j.hasNext())
    {
        j.next();
        QDomElement pair = document.createElement(XML_ADDITIONALDATA);
        pair.setAttribute(XML_ADNAME, j.key());
        pair.setAttribute(XML_ADVALUE, j.value());
        element.appendChild(pair);
    }

    return element;
}
