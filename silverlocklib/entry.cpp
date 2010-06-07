#include "entry.h"

Entry::Entry()
    : m_uuid(QUuid::createUuid())
{
}

Entry::Entry(const QString &title)
    : m_uuid(QUuid::createUuid()), m_title(title)
{
}

QUuid Entry::uuid() const
{
    return this->m_uuid;
}

QString Entry::title() const
{
    return this->m_title;
}

void Entry::setTitle(const QString &title)
{
    this->m_title = title;
}

QUrl Entry::url() const
{
    return this->m_url;
}

void Entry::setUrl(const QUrl &url)
{
    this->m_url = url;
}

QString Entry::username() const
{
    return this->m_username;
}

void Entry::setUsername(const QString &username)
{
    this->m_username = username;
}

QString Entry::password() const
{
    return this->m_password;
}

void Entry::setPassword(const QString &password)
{
    this->m_password = password;
}

QString Entry::emailAddress() const
{
    return this->m_emailAddress;
}

void Entry::setEmailAddress(const QString &emailAddress)
{
    this->m_emailAddress = emailAddress;
}

QString Entry::notes() const
{
    return this->m_notes;
}

void Entry::setNotes(const QString &notes)
{
    this->m_notes = notes;
}

QHash<QString, QString>& Entry::recoveryInfo()
{
    return this->m_recoveryInfo;
}

QHash<QString, QString>& Entry::additionalData()
{
    return this->m_additionalData;
}
