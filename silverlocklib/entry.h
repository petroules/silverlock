#ifndef ENTRY_H
#define ENTRY_H

#include "silverlocklib_global.h"
#include "databasenode.h"
#include <QtCore>

class Group;

class SILVERLOCKLIBSHARED_EXPORT Entry : public DatabaseNode
{
    Q_OBJECT
    Q_PROPERTY(QUrl url READ url WRITE setUrl)
    Q_PROPERTY(QString username READ username WRITE setUsername)
    Q_PROPERTY(QString password READ password WRITE setPassword)
    Q_PROPERTY(QString notes READ notes WRITE setNotes)

    friend class DatabaseReader;

public:
    explicit Entry(const QString &title = QString(), Group *parent = NULL);
    ~Entry();
    QString username() const;
    void setUsername(const QString &username);
    QString password() const;
    void setPassword(const QString &password);
    QUrl url() const;
    void setUrl(const QUrl &url);
    QString notes() const;
    void setNotes(const QString &notes);
    const QMap<QString, QString>& recoveryInfo() const;
    const QMap<QString, QString>& customFields() const;
    void insertRecoveryInfo(const QString &question, const QString &answer);
    int removeRecoveryInfo(const QString &question);
    void clearRecoveryInfo();
    void insertCustomField(const QString &key, const QString &value);
    int removeCustomField(const QString &key);
    void clearCustomFields();
    QDomElement toXml(QDomDocument &document) const;
    Entry* createCopy() const;

protected:
    void attachToList();
    void detachFromList();
    void fromXml(const QDomElement &element);

private:
    QUrl m_url;
    QString m_username;
    QString m_password;
    QString m_notes;
    QMap<QString, QString> m_recoveryInfo;
    QMap<QString, QString> m_customFields;
};

#endif // ENTRY_H
