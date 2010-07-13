#ifndef ENTRY_H
#define ENTRY_H

#include "silverlocklib_global.h"
#include "itemnode.h"
#include <QtCore>

class Database;
class Group;
class GroupNode;

class SILVERLOCKLIBSHARED_EXPORT Entry : public ItemNode
{
    Q_OBJECT
    Q_PROPERTY(QUrl url READ url WRITE setUrl)
    Q_PROPERTY(QString username READ username WRITE setUsername)
    Q_PROPERTY(QString password READ password WRITE setPassword)
    Q_PROPERTY(QString emailAddress READ emailAddress WRITE setEmailAddress)
    Q_PROPERTY(QString notes READ notes WRITE setNotes)

public:
    explicit Entry(const QString &title = QString(), GroupNode *parent = NULL);
    ~Entry();
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
    const QHash<QString, QString>& recoveryInfo() const;
    const QHash<QString, QString>& additionalData() const;
    void insertRecoveryInfo(const QString &key, const QString &value);
    int removeRecoveryInfo(const QString &key);
    void clearRecoveryInfo();
    void insertAdditionalData(const QString &key, const QString &value);
    int removeAdditionalData(const QString &key);
    void clearAdditionalData();
    QDomElement toXml(QDomDocument &document) const;

protected:
    void attachToList();
    void detachFromList();

private:
    QUrl m_url;
    QString m_username;
    QString m_password;
    QString m_emailAddress;
    QString m_notes;
    QHash<QString, QString> m_recoveryInfo;
    QHash<QString, QString> m_additionalData;
};

#endif // ENTRY_H
