#ifndef ENTRY_H
#define ENTRY_H

#include "silverlocklib_global.h"
#include <QtCore>

class SILVERLOCKLIBSHARED_EXPORT Entry
{
public:
    Entry();
    Entry(const QString &title);
    QUuid uuid() const;
    QString title() const;
    void setTitle(const QString &title);
    QUrl url() const;
    void setUrl(const QUrl &url);
    QString username() const;
    void setUsername(const QString &username);
    QString password() const;
    void setPassword(const QString &password);
    QString emailAddress() const;
    void setEmailAddress(const QString &emailAddress);
    QString notes() const;
    void setNotes(const QString &notes);
    QHash<QString, QString>& recoveryInfo();
    QHash<QString, QString>& additionalData();

private:
    QUuid m_uuid;
    QString m_title;
    QUrl m_url;
    QString m_username;
    QString m_password;
    QString m_emailAddress;
    QString m_notes;
    QHash<QString, QString> m_recoveryInfo;
    QHash<QString, QString> m_additionalData;
};

#endif // ENTRY_H
